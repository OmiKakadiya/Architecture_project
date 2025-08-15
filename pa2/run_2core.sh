if [ $# -lt 4 ]; then
    echo "Usage: $0 <binary> <num1> <num2> <file_name>"
    exit 1
fi

# provide the path of both the trace file
TRACE_DIR1="/home/surajp2909/Sem1/Sem2/cs773/traces"  #path of the first trace file (excluding the trace file name)
TRACE_DIR2="/home/surajp2909/Sem1/Sem2/cs773/traces"  #path of the second trace file (excluding the trace file name)
binary=${1}
num1=${2}
num2=${3}
file_name=${4}

trace1=`sed -n ''$num1'p' workloads.txt | awk '{print $1}'`
trace2=`sed -n ''$num2'p' workloads.txt | awk '{print $1}'`

echo $trace1
echo $trace2


mkdir -p results #change the name of your folder as required 
(${binary} -warmup_instructions 50000000 -simulation_instructions 50000000 -traces ${TRACE_DIR1}/${trace1} ${TRACE_DIR2}/${trace2}) > results/mix-${num1}-${num2}_${file_name}.txt & #change the name of the folder as required