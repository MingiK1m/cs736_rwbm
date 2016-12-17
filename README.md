<b>Steps to measure</b>

1) ./bm write [block_size] [iter_count] [wt_log_path] off

2) [Reboot]

3) ./bm read [block_size] [iter_count] [rd_log_path] off

4) ./bm remove [block_size] [iter_count] [dont_care] off

5) [Reboot]

6) ./bm write [block_size] [iter_count] [wt_fsync_log_path] on

7) ./bm remove [block_size] [iter_count] [dont_care] on

----

ex)

./bm write 4096 10000 wrt_log off

reboot

./bm read 4096 10000 rd_log off

./bm remove 4096 10000 a off

reboot

./bm write 4096 10000 wrt_fsync_log on

./bm remove 4096 10000 a on

----

file system block size can be obtained using command below.

sudo blockdev --getbsz [partition]

----

ex)

mingi@mingi-Laptop:~$ sudo blockdev --getbsz /dev/sda3

4096
