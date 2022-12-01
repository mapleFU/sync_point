//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include "sync_point_impl.h"

#ifndef NDEBUG

void SyncPoint::Data::LoadDependency(const std::vector<SyncPoint::SyncPointPair>& dependencies) {
    std::lock_guard<std::mutex> lock(mutex_);
    successors_.clear();
    predecessors_.clear();
    cleared_points_.clear();
    for (const auto& dependency : dependencies) {
        successors_[dependency.predecessor].push_back(dependency.successor);
        predecessors_[dependency.successor].push_back(dependency.predecessor);

        /*
        point_filter_.Add(dependency.successor);
        point_filter_.Add(dependency.predecessor);
        */
    }
    cv_.notify_all();
}

void SyncPoint::Data::LoadDependencyAndMarkers(
        const std::vector<SyncPoint::SyncPointPair>& dependencies,
        const std::vector<SyncPoint::SyncPointPair>& markers) {
    std::lock_guard<std::mutex> lock(mutex_);
    successors_.clear();
    predecessors_.clear();
    cleared_points_.clear();
    markers_.clear();
    marked_thread_id_.clear();
    for (const auto& dependency : dependencies) {
        successors_[dependency.predecessor].push_back(dependency.successor);
        predecessors_[dependency.successor].push_back(dependency.predecessor);
        /*
        point_filter_.Add(dependency.successor);
        point_filter_.Add(dependency.predecessor);
         */
    }
    for (const auto& marker : markers) {
        successors_[marker.predecessor].push_back(marker.successor);
        predecessors_[marker.successor].push_back(marker.predecessor);
        markers_[marker.predecessor].push_back(marker.successor);
        /*
        point_filter_.Add(marker.predecessor);
        point_filter_.Add(marker.successor);
         */
    }
    cv_.notify_all();
}

bool SyncPoint::Data::PredecessorsAllCleared(const std::string& point) {
    for (const auto& pred : predecessors_[point]) {
        if (cleared_points_.count(pred) == 0) {
            return false;
        }
    }
    return true;
}

void SyncPoint::Data::ClearCallBack(const std::string& point) {
    std::unique_lock<std::mutex> lock(mutex_);
    while (num_callbacks_running_ > 0) {
        cv_.wait(lock);
    }
    callbacks_.erase(point);
}

void SyncPoint::Data::ClearAllCallBacks() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (num_callbacks_running_ > 0) {
        cv_.wait(lock);
    }
    callbacks_.clear();
}

void SyncPoint::Data::Process(std::string_view point, void* cb_arg) {
    if (!enabled_) {
        return;
    }

    // Use a filter to prevent mutex lock if possible.
    /*
    if (!point_filter_.MayContain(point)) {
        return;
    }
     */

    // Must convert to std::string for remaining work.  Take
    //  heap hit.
    std::string point_string(point);
    std::unique_lock<std::mutex> lock(mutex_);
    auto thread_id = std::this_thread::get_id();

    auto marker_iter = markers_.find(point_string);
    if (marker_iter != markers_.end()) {
        for (auto& marked_point : marker_iter->second) {
            marked_thread_id_.emplace(marked_point, thread_id);
            /*
            point_filter_.Add(marked_point);
             */
        }
    }

    if (DisabledByMarker(point_string, thread_id)) {
        return;
    }

    while (!PredecessorsAllCleared(point_string)) {
        cv_.wait(lock);
        if (DisabledByMarker(point_string, thread_id)) {
            return;
        }
    }

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
}

#endif
