#!/usr/local/bin/bash

#{ set $List; shift $(($#/2)); bot_half="$@"; top_half=${top_half%"$bot_half"}; }

# tst() { cmd="$1"; shift; target=$1; shift; rsh $target "$cmd" "$@"; }

# v0.11

DIST=test/dist.sh

copy0() {
	return 0
}

copy1() {
	echo "rsync -v $FILE $1:$FILE"
	rsync -v $FILE $1:$FILE
}

run() {
	target=$1
#	echo "target = $target"
	shift || return 0
#echo "rsh $target \"$cmd\" \"$@\""
	$COPY $target
	rsh -n $target "$DIST \"$cmd\" \"$@\" < /dev/null > /dev/null 2>&1 &"
#	rsh $target "cat > test/.cmd"
#	rsh -n $target "test/dist.sh \"$@\" < test/.cmd > /dev/null 2>&1 &"
}

copy() {
	target=$1
#	echo "target = $target"
	shift || return 0
#echo "rsh $target \"$cmd\" \"$@\""
#	rsh $target test/dist.sh "$@"
	rsh -n $target "$DIST copy $me $FILE \"$@\" < /dev/null > /dev/null 2>&1 &"
}

me=`hostname -s`
MyLog=".distlog"
echo -n > $MyLog

echo "`hostname -s`> \$0=$0" >>$MyLog

COPY=copy0
FILE=

case $1 in
    copy\ *)
		FILE=${1#copy }
		shift || return 1
		COPY=copy1
		cmd="copy $FILE"
		;;
    *)
		cmd="$1"
		shift || return 0
		;;
esac



echo "`hostname -s`> arg=$@" >>$MyLog
echo "`hostname -s`> cmd=$cmd" >>$MyLog

if [ $# -gt 0 ]; then
	List="$@"
	{ set $List; shift $(($#/2)); bot_half="$@"; top_half=${List%"$bot_half"}; }

	run $top_half >>$MyLog
	run $bot_half >>$MyLog
fi

#eval $cmd 2>&1 | rsh fryazino "cat > test/agents/`hostname -s`" &
#eval $cmd 2>&1 | rsh fryazino 'cd test/agents && x=`mktemp tmp/distlog.XXXXXX` && cat > $x && mv $x '"$me" &
# try random sleep
#sleep $((`head -c1 < /dev/random | od -An -tu1` % 8))
#eval $cmd 1>distlog 2>&1; rsync distlog fryazino:test/agents/$me && rm distlog

#printf "%d\t%d\n%s" $? $$ "$cmd" | rsh fryazino "cat > test/agents/`hostname -s`"

[ $COPY = copy1 ] || eval $cmd 1>>$MyLog 2>&1

[ -s "$MyLog" ] && { rsync $MyLog fryazino:test/agents/$me && rm -f $MyLog; } || rm -f $MyLog
#wait

