#!/bin/sh


run() {
	echo "run()"
	OPTIND=1
	while getopts 'xy:z:' OPTION
	do
		echo "OPTION = $OPTION"
		case $OPTION in
			x)	REPLACE="--replace"
				echo "-x was triggered!" 
				;;
			y)	DATE_FROM="$OPTARG"
				echo "-y was triggered!" $OPTARG
				;;
			z)	DATE_TO="$OPTARG"
				echo "-z was triggered!" $OPTARG
				;;
			\?)
				echo "Invalid option: -$OPTARG"
			#	echo "-$OPTARG"
				;;
			:)
				echo "Option -$OPTARG requires an argument."
				exit 1
		esac
	done
	shift $(($OPTIND - 1))

	process_commands "$@"
}

dryRun=
logLevel=6

Options() {
	return 1
}

BaseOptions() {
	case $1 in
		l|log-level)	
			logLevel=$2
			logLevelOverride=1
			case $logLevel in
				1|fatal|FATAL)	logLevel=1
					;;
				2|error|ERROR)	logLevel=2
					;;
				3|warning|WARN|WARNING)	logLevel=3
					;;
				4|error|ERROR)          logLevel=4
					;;
				5|notice|NOTICE)        logLevel=5
					;;
				6|info|information|INFO) logLevel=6
					;;
				7|debug|DEBUG)          logLevel=7
					;;
				8|trace|TRACE)          logLevel=8
					;;
				*)
					echo "Invalid log level: $logLevel"
					exit 1
					;;
			esac
			;;
		n|dry-run)
			dryRun="--dry-run"
			;;
		d|working-dir)
			WMC_WD=$2
			;;
		*)	#if [ ! -z EXTENDED_OPTIONS ]; then eval "$EXTENDED_OPTIONS $1 $2" || return 1; fi
			Options $1 $2 || return $?
	esac
}

ProcessOptions() {
	[ $# -gt 0 ] || return 0

	OPTIND=1
	while getopts 'nl:m:-:' OPTION
	do
		
		case $OPTION in
			-)	#echo "-- was triggered: $OPTARG"
				longOpt=`echo $OPTARG | sed -e 's/=.*//'`
				longArg=`echo $OPTARG | sed -ne '/=/ s/.*=// p'`
				BaseOptions $longOpt $longArg || { echo "Invalid option: --$longOpt"; exit 1; }
				;;
			\?)	#echo "Invalid option: -$OPTARG"
				exit 1
				;;
			:)	#echo "Option -$OPTARG requires an argument."
				exit 1
				;;
		esac
	done
	shift $(($OPTIND - 1))

	ProcessCommands "$@"
	return $?
}

# private override
Options() {
	case $1 in
		m|master)
			echo $2
			parentHost=`echo $2 | sed -e 's/:.*//'`
			parentDir=`echo $2 | sed -ne '/:/ s/.*:// p'`
			echo $parentHost $parentDir
			;;
		*)	return 1
	esac
}

Collect() {
	echo "Collect"
#	return 42
	ProcessCommands "$@"
}

Merge() {
	echo "Merge"
	ProcessCommands "$@"
}

usage() {
	echo "Available commands: collect, merge"
}

ProcessCommands() {
	[ $# -gt 0 ] || return 0
	
	command=$1
	shift
	
	case $command in
		collect)	Collect "$@"
			;;
		merge)		Merge "$@"
			;;
		*)	echo "Unknown command $1"
			usage
			exit 1
			;;
	esac
	return $?
}

EXTENDED_OPTIONS=myOptions
ProcessOptions "$@"
exit $?

