#!/bin/bash
#SBATCH --job-name=axpy-bench
#SBATCH --partition=Centaurus
#SBATCH --time=00:20:00
#SBATCH --output=logs/%x-%j.out
#SBATCH --error=logs/%x-%j.err
#SBATCH --mem=20GB

set -euo pipefail
cd "$SLURM_SUBMIT_DIR"
mkdir -p logs results

echo "Running on node: $(hostname)"
echo "Start time: $(date)"

# Compile
gcc -O3 -pthread axpy_full.c -lm -o axpy_full

OUT="results/axpy_times_${SLURM_JOB_ID}.csv"
echo "N,threads,serial_ms,thread_ms" > "$OUT"

# Match your table
sizes=(1000000 10000000 100000000 1000000000)
threads=(1 2 4 8 16 32 36)

for N in "${sizes[@]}"; do
  for T in "${threads[@]}"; do
    echo "Running N=$N T=$T"

    # Run program, parse the serial and threaded ms from output
    # Output lines look like:
    # AXPY-serial:        1.200008 ...
    # AXPY-4 threads:     1.600003 ...
    out=$(./axpy_full "$N" "$T")

    serial_ms=$(echo "$out" | awk '/AXPY-serial:/ {print $2}')
    thread_ms=$(echo "$out" | awk -v t="$T" '$0 ~ "AXPY-"t" " {print $3}')

    echo "$N,$T,$serial_ms,$thread_ms" >> "$OUT"
  done
done

echo "Results written to $OUT"
echo "End time: $(date)"
