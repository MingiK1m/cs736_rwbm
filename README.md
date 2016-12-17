Steps to measure

1) ./bm write <block_size> <iter_count> <wt_log_path>
2) [Reboot]
3) ./bm read <block_size> <iter_count> <rd_log_path>
4) ./bm remove <block_size> <iter_count> <dont_care>

ex)
./bm write 4096 10000 wrt_log
reboot
./bm read 4096 10000 rd_log
./bm remove 4096 10000 a

file system block size can be obtained using command below.
sudo blockdev --getbsz <partition>

ex)
mingi@mingi-Laptop:~$ sudo blockdev --getbsz /dev/sda3
4096
