# SyncPoint

SyncPoint is pasted from https://github.com/facebook/rocksdb/blob/main/test_util/sync_point.h . It's very helpful when testing multi-thread programs.

Here are some differences from rocksdb::SyncPoint:
* No KillPoint
* No BloomFilter in SyncPoint
* Namespace is different

Thanks rocksdb!

## Notice

You'd better not throw exception in `SyncPoint`, because it's not exception safety:

```c++
    auto callback_pair = callbacks_.find(point_string);
    if (callback_pair != callbacks_.end()) {
        num_callbacks_running_++;
        mutex_.unlock();
        callback_pair->second(cb_arg);
        mutex_.lock();
        num_callbacks_running_--;
    }
    cleared_points_.insert(point_string);
    cv_.notify_all();
```

If you throw exception in callback, it will throw exception without dec `num_callbacks_running_`.