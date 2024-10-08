# Fuzzing the mavlink C library

```bash
# Prepare seeds
# You first need to place some MAVLink recording at `recording.mavlink`.
rm -rf seeds
mkdir -p seeds
cd seeds
cp ../recording.mavlink .
split -b 5000 recording.mavlink
rm recording.mavlink
cd ..

# Build
git clone https://github.com/mavlink/c_library_v1
git clone https://github.com/mavlink/c_library_v2
apt-get update
apt-get install -y build-essential afl++
afl-cc -I generated/include -o fuzztarget fuzztarget.c
gcc -I generated/include -o fuzztarget fuzztarget.c
# Test on some seed
./fuzztarget seeds/xbe

# Set up fuzzing system params
cd /sys/devices/system/cpu
echo performance | sudo tee cpu*/cpufreq/scaling_governor
echo core | sudo tee /proc/sys/kernel/core_pattern

# Run fuzzer
AFL_SKIP_CPUFREQ=1 afl-fuzz -i seeds/ -o fuzz_out/ -- ./fuzztarget '@@'
```
