#!/bin/sh
# Utilities to process DB-related variables from WMC_CONFIG

# rebuild $HOST_DB_URLS as shell list (space-separated)
db_list() {
	IFS="${IFS=   }"; save_ifs="$IFS"; IFS=','
	urls=
	for s in $HOST_DB_URLS; do
	  urls+="$s "
	done
    echo $urls
	IFS="$save_ifs"
}

# echoes the number of DBs
db_count() {
	db_count_ `db_list`
}

# echoes [0, N_DBs) list of integers, e.g. 0 1 2 3 if we have 4 DBs
db_range() {
	range=
	n_db=`db_count`
	if [ $n_db -gt 10 ]; then
		printf >&2 "Not implemented for db_count \n" 
		return 1
	fi
	
	k=0
	while [ $k -lt $n_db ]; do
		range+="$k "
		k=$(($k+1))
	done
	echo $range
	return 0
}

# db_nth <user|pass|host|name|url|auth> N
#	- echoes variable value for Nth DB 
#	- auth is complete authorization cmdline parameter, e.g. "-uhost_rw --password=*** -hwmc-05.yandex.ru -Dhost_05"
# indexes are zero-based, e.g. [0..3]; corresponds to Hash(hostName) % NumDBs
db_nth() {
	[ -z $2 ] && { printf >&2 "Parameter missed: DB index [%s]\n" $2; return 1; }
	[ "$2" -ge 0 ] && [ "$2" -lt "`db_count`" ] || { printf >&2 "Illegal DB index [%s]\n" $2; return 2; }
	
	case "$1" in
		user|pass|host|name|url|auth)
			eval db_nth_${1}_ $2
			return $?
			;;
		*)	
			printf >&2 "Illegal parameter [%s]\n" $1
			return 2
	esac
}


# ==============================================================================
# Unchecked, internal implementation. Not for direct use
# ==============================================================================
# unchecked, internal function, can be used as db_count_ `db_list`
db_count_() {
	echo "$#"
}

# unchecked, internal
db_nth_url_() {
	k=0
	for url in `db_list`; do
		[ ".$k" = ".$1" ] && echo $url && return 0
		k=$(($k+1))
	done
	printf >&2 "Illegal DB index [%s]\n" $1
	return 2
}

# unchecked, internal
db_nth_name_() {
	eval x=\$$"HOST${1}_DB_NAME"
	echo $x
	return 0
}

# unchecked, internal
db_nth_host_() {
	eval x=\$$"HOST${1}_DB_HOST"
	echo $x
	return 0
}

# unchecked, internal
db_nth_user_() {
	eval x=\$$"HOST${1}_DB_USER"
	echo $x
	return 0
}

# unchecked, internal
db_nth_pass_() {
	eval x=\$$"HOST${1}_DB_PASSWORD"
	echo $x
	return 0
}

# Complete authorization command line. Unchecked, internal
db_nth_auth_() {
	eval u=\$$"HOST${1}_DB_USER"
	eval p=\$$"HOST${1}_DB_PASSWORD"
	eval h=\$$"HOST${1}_DB_HOST"
	eval d=\$$"HOST${1}_DB_NAME"
	echo "-u$u --password=$p -h$h -D$d"
	return 0
}

# ==============================================================================

# self-test aka POST
test_() {
	[ -z $WMC_CONFIG ] && printf >&2 "WMC_CONFIG not defined \n" && exit 2
	. $WMC_CONFIG || exit 1
	
	DB_URLS=`db_list`
	DB_COUNT=`db_count`

	printf "DB_COUNT = $DB_COUNT\n"
	
	for x in $DB_URLS; do echo $x; done
	
	DBs=`db_range`
	for x in $DBs; do echo -n "$x,"; done
	echo	# \n
	
	db_nth url   || printf >&2 "Error $? [%s]\n" 'db_nth url  '
	db_nth url x || printf >&2 "Error $? [%s]\n" 'db_nth url x'
	db_nth url 0 || printf >&2 "Error $? [%s]\n" 'db_nth url 0'
	db_nth url 4 || printf >&2 "Error $? [%s]\n" 'db_nth url 4'
	
	db_nth name 
	db_nth name x
	db_nth name 1
	db_nth name 4
	
	db_nth host 0
	db_nth pass 0
	
	echo "mysql `db_nth auth 1` --no-auto-rehash -e 'show create table tbl_key_regions;'"
	cmd_arg=`db_nth auth 1` && \
	mysql $cmd_arg --no-auto-rehash -e "show create table tbl_key_regions;"
	
	cmd_arg=`db_nth auth 6` && \
	mysql $cmd_arg --no-auto-rehash -e "show create table tbl_key_regions;"
	
}

test_

