#!/bin/sh -e

PATH=/bin:/usr/bin:/usr/local/bin
export PATH


#-- Subroutines --------------------------------------------------------

err()
{       
	local _exitval

	_exitval=$1
	shift

	echo 1>&2 "[$(date "+%F %T")] ERROR: $*"
	exit $_exitval
}


#-- Variables ----------------------------------------------------------

log_dir="$(dirname $0)/log"
log_out_prefix="${log_dir}/log_out_"
log_err_prefix="${log_dir}/log_err_"

hostname=$(hostname)
master="mysql --defaults-group-suffix=clean_rw -h ${hostname} -NBAe"
max_fails_on_tbl=16

case $hostname in
	wmc-03.yandex.ru) threads=6 ;;
	*) threads=10 ;;
esac

tbl_prefix="tbl_queries"
limit=1000
query_tpl="SET sql_log_bin = 0; DELETE FROM ${tbl_prefix}<part> WHERE query_id IN ( SELECT temp FROM ( SELECT DISTINCT(q.query_id) AS temp FROM ${tbl_prefix}<part> q LEFT JOIN tbl_query_stats<part> qs ON q.query_id = qs.query_id LEFT JOIN tbl_customquery_history<part> cqh ON q.query_id = cqh.query_id LEFT JOIN tbl_custom_queries<part> cq ON q.query_id = cq.query_id WHERE qs.query_id IS NULL AND cqh.query_id IS NULL AND cq.query_id IS NULL LIMIT ${limit}) t); SELECT ROW_COUNT(); SET sql_log_bin = 1;"


#-- Main ---------------------------------------------------------------

trap "echo Canceled ..; exit 1" 2

db=$($master "show databases" | grep host_)
if [ -z "${db}" ]; then
	err 1 "Can't get database name !"
fi

tbls=$($master "show tables like '${tbl_prefix}%'" $db)
if [ -z "${tbls}" ]; then
	err 1 "Can't get tables !"
fi

for tbl in $tbls; do
	num=$(echo $tbl | sed -Ee "s/${tbl_prefix}0{0,2}//")
	rest=$(($num % $threads))
	eval pool_${rest}="\$pool_${rest}\${pool_${rest}:+ }\$tbl"
done

for pool_num in $(jot $threads 0); do
	eval pool=\$pool_${pool_num}
	log_out="${log_out_prefix}_${pool_num}"
	log_err="${log_err_prefix}_${pool_num}"

	for tbl in $pool; do
		tbl_out="${log_dir}/${tbl}"
		part=${tbl#${tbl_prefix}}
		query=$(echo "${query_tpl}" | \
			sed -e "s/<part>/${part}/g;")

		one_tbl_run_num=0
		one_tbl_err_num=0

		while true; do
			echo -n "[$(date "+%F %T")]" \
				"${part}->${one_tbl_run_num} ... "

			if ! $master "${query}" $db > $tbl_out
			then
				one_tbl_err_num=$((${one_tbl_err_num} + 1))
				if [ $one_tbl_err_num -gt $max_fails_on_tbl ]
				then
					err 1 "Max fails on table ${tbl} !"
				else
					continue
				fi
			fi


			rows=$(cat $tbl_out | grep -Eo "[0-9]+")

			if [ ${rows:-$(($limit + 1))} -lt $limit ]
			then
				echo "finished (rows: ${rows})."
				break
			fi

			echo "done (rows: ${rows})"

			one_tbl_run_num=$((${one_tbl_run_num} + 1))
		done

	done >$log_out 2>$log_err &

done

wait

