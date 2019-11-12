// Copyright Vladimir Prus 2002-2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

/* Shows how to use both command line and config file. */

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#define TraceX(x)	std::cout << __func__ << "> "#x" = " << x << std::endl

#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <map>
#include <functional>
using namespace std;

//#include <boost/foreach.hpp>
//#define foreach  BOOST_FOREACH

//=------------------------------------------------------------------------------
// Debug output helpers
//=------------------------------------------------------------------------------
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
//	foreach( const T& _t, v ) os << _t << endl;
	typename std::vector<T>::const_iterator q;
	for (typename std::vector<T>::const_iterator p = v.begin(); p != v.end(); ++p) {
		os << *p << endl;
	}
    return os;
}

template<class Key, class Value>
ostream& operator<<(ostream& os, const map<Key,Value>& v)
{
//	typedef typename map<Key,Value>::value_type	T;
//	foreach( const T& _t, v ) {
	for (typename map<Key,Value>::const_iterator p = v.begin(); p != v.end(); ++p) {
	    os << p->first << " -> " << p->second << endl;
	}
    return os;
}
/*
template <typename charT>
ostream& operator<<(ostream& os, const po::basic_option<charT>& x)
{
	os << x.string_key << " :";
	foreach( const string& _s, x.value) os << ' ' << _s;
	return os;
}
*/
ostream& operator<<(ostream& os, const po::option& x)
{
	os << x.string_key << " :";
	for (vector<string>::const_iterator p = x.value.begin(); p != x.value.end(); ++p) {
		os << ' ' << *p;
	}
	return os;
}

//===============================================================================

//namespace wmc {

class OptionDictionary : public map<string, string>
{
public:
	explicit OptionDictionary(const vector<po::option>& dict);
	string interpret(const string& ident) const;
};

//class DictExpansion

//! Sort of expansion / evaluation function for shell-like syntax (simplified)
template<class ExpandFunction>
string eval(const string& s, const ExpandFunction& exp)
{
//	if (s.find('$') == string::npos || dict.empty()) return s;
	string res;

	bool isIdent = false;
	bool isString = false;
	bool isLiteral = false;
	bool isEscape = false;
	bool isIdBracket = false;
	string ident;	// var identifier
	string value;	// var value

	string::const_iterator se = s.end();
	for (string::const_iterator p = s.begin(); p != se; ++p) {
		char c = *p;

		if (isIdent) {
			if (isalnum(c) || c == '_') {
				ident += c;
				continue;
			} else { // identifier ends
				res += exp(ident);
				isIdent = false;
				ident.erase();
			}
			// fall through:
		}

		if (isEscape) {
			res += c;
			isEscape = false;
			continue;
		}

		if (isLiteral) {	// Literal is exact representation - no translation, no escaping, etc
			if (c == '\'') {
				isLiteral = false;
			} else {
				res += c;
			}
			continue;
		}

		if (c == '$') {
			isIdent = true;
		} else if (c == '\'') {
			isLiteral = true;
		} else if (c == '\\') {
			isEscape = true;
		} else if (c == '"') {
			// nothing
		} else {
			res += c;
		}
	}

	if (isIdent) { // identifier at the end of the string
		res += exp(ident);
	}

	return res;
}

struct OptionDictionaryExp
{
	OptionDictionaryExp(const OptionDictionary& self) : self_(self) {}
	string operator()(const string& s) const { return self_.interpret(s); }
	const OptionDictionary& self_;
};

OptionDictionary::OptionDictionary(const vector<po::option >& dict)
{
//	foreach( const po::option& x, dict ) {
//	//	(*this)[x.string_key] = x.value[0];
//	//	(*this)[x.string_key] = eval(x.value[0], *this);	// resolve sequentially
//		(*this)[x.string_key] = eval(x.value[0], OptionDictionaryExp(*this));
//	//	(*this)[x.string_key] = eval(x.value[0], bind1st(mem_fun(&OptionDictionary::interpret), *this));
//	}
	for (vector<po::option>::const_iterator p = dict.begin(); p != dict.end(); ++p) {
		(*this)[p->string_key] = eval(p->value[0], OptionDictionaryExp(*this));
	}
}

string OptionDictionary::interpret(const string& ident) const
{
	const_iterator found = this->find(ident);
	if (found != this->end()) {
	//	return eval(found->second, *this);	// recursive
		return found->second;               // non-recursive
	} else {
		cerr << "Parsing error: \"" << ident << "\" not found in dictionary" << endl;
		return string("$").append(ident);
	}
}

/**
 *	Try to load config file with shell-like (some simplified) syntax
 * 	Performs simple variable expansion based on previous lines AND environment variables.
 */

//ConfigPreload
/*
class ConfigLoader
	: protected OptionDictionary
{
public:
	ConfigLoader() { }
	explicit ConfigLoader(istream& configFile);

	void chain(istream& configFile);

	const string& operator[](const string& key) const;
};


ConfigLoader::ConfigLoader(istream& configFile)
{
	chain(configFile);
}

void
ConfigLoader::chain(istream& configFile)
{
	po::options_description desc;
    bool allow_unregistered = true;
    po::parsed_options cfg_opts = parse_config_file(configFile, desc, allow_unregistered);

	// Resolve sequentially, line by line
	// Replace existing entries, like interpreter does
	foreach( const po::option& x, cfg_opts.options ) {
		(*this)[x.string_key] = eval(x.value[0], *this);	// resolve sequentially
	}

}
*/

//} // namespace wmc


int main(int ac, char* av[])
{
    try {
    	struct {
    		int	jobs;
    		string WMC_OFFLINE_DEST_SERVER;
    	} opt;
        int o_level;

        // Declare a group of options that will be
        // allowed only on command line
        po::options_description generic("Generic options");
        generic.add_options()
            ("version,v", "print version string")
            ("help", "produce help message")
            ;

        // Declare a group of options that will be
        // allowed both on command line and in
        // config file
        po::options_description config("Configuration");
        config.add_options()
            ("jobs,j", po::value<int>(&opt.jobs)->default_value(1),
                  "Run up to N parallel tasks or threads (default is 1)")
            ("WMC_OFFLINE_DEST_SERVER", po::value<string>(&opt.WMC_OFFLINE_DEST_SERVER),
                  "rsync target to store resulting files")

            ("optimization", po::value<int>(&o_level)->default_value(10),
                  "optimization level")
            ("include-path,I",
                 po::value< vector<string> >()->composing(),
                 "include path")
            ;

        // Hidden options, will be allowed both on command line and
        // in config file, but will not be shown to the user.
        po::options_description hidden("Hidden options");
        hidden.add_options()
            ("input-file", po::value< vector<string> >(), "input file")
            ;


        po::options_description cmdline_options;
        cmdline_options.add(generic).add(config).add(hidden);

        po::options_description config_file_options;
        config_file_options.add(config).add(hidden);

        po::options_description visible("Allowed options");
        visible.add(generic).add(config);

        po::positional_options_description p;
        p.add("input-file", -1);

        po::variables_map opts;
        store(po::command_line_parser(ac, av).
              options(cmdline_options).positional(p).run(), opts);

        ifstream ifs("multiple_sources.properties");
        store(parse_config_file(ifs, config_file_options), opts);
        notify(opts);

        ifstream ifs2("wmconsole.cfg");
        bool allow_unregistered = true;
        po::parsed_options cfg_opts = parse_config_file(ifs2, config_file_options, allow_unregistered);
	//	std::cout << cfg_opts.options;
		OptionDictionary dict(cfg_opts.options);
    	std::cout << dict;
        store(cfg_opts, opts);
        notify(opts);

   //     collect_unrecognized(const std::vector< basic_option< charT > > &, collect_unrecognized_mode);

        if (opts.count("help")) {
            cout << visible << "\n";
            return 0;
        }

        if (opts.count("version")) {
            cout << "Multiple sources example, version 1.0\n";
            return 0;
        }

        if (opts.count("include-path"))
        {
            cout << "Include paths are: "
                 << opts["include-path"].as< vector<string> >() << "\n";
        }

        if (opts.count("input-file"))
        {
            cout << "Input files are: "
                 << opts["input-file"].as< vector<string> >() << "\n";
        }

        cout << "Optimization level is " << o_level << "\n";

        TraceX(opt.jobs);
        TraceX(opt.WMC_OFFLINE_DEST_SERVER);

	//	foreach (const po::option& _o, cfg_opts.options) {
	//		string sss = eval(_o.value[0], dict);
	//		cout << _o.string_key << " => " << sss << endl;
	//	}
/*		{
        	string sss = eval("$BACKUP_ROOT/$segment/$BACKUP_SUFFIX", dict);
        	TraceX(sss);
		}*/

     //   cout << "WMCOFFLINEMANAGER_HOME: " << opts["WMCOFFLINEMANAGER_HOME"].as< vector<string> >();
     //   string sss = eval(opts["WMCOFFLINEMANAGER_HOME"].as< vector<string> >()[0], dict);
     //   cout << "WMCOFFLINEMANAGER_HOME> " << sss << endl;
   }
    catch(exception& e)
    {
        cout << "[ERROR] Exception: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
