# SyncPoint

SyncPoint is pasted from https://github.com/facebook/rocksdb/blob/main/test_util/sync_point.h . It's very helpful when testing multi-thread programs.

Here are some differences from rocksdb::SyncPoint:
* No KillPoint
* No BloomFilter in SyncPoint
* Namespace is different

Thanks rocksdb!