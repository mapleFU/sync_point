//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include "sync_point.h"

#include "sync_point_impl.h"

#ifndef NDEBUG

SyncPoint* SyncPoint::GetInstance() {
    static SyncPoint sync_point;
    return &sync_point;
}

SyncPoint::SyncPoint() : impl_(new Data) {}

SyncPoint::~SyncPoint() {
    delete impl_;
}

void SyncPoint::LoadDependency(const std::vector<SyncPointPair>& dependencies) {
    impl_->LoadDependency(dependencies);
}

void SyncPoint::LoadDependencyAndMarkers(
        const std::vector<SyncPointPair>& dependencies,
        const std::vector<SyncPointPair>& markers) {
    impl_->LoadDependencyAndMarkers(dependencies, markers);
}

void SyncPoint::SetCallBack(const std::string& point,
                            const std::function<void(void*)>& callback) {
    impl_->SetCallBack(point, callback);
}

void SyncPoint::ClearCallBack(const std::string& point) {
    impl_->ClearCallBack(point);
}

void SyncPoint::ClearAllCallBacks() {
    impl_->ClearAllCallBacks();
}

void SyncPoint::EnableProcessing() {
    impl_->EnableProcessing();
}

void SyncPoint::DisableProcessing() {
    impl_->DisableProcessing();
}

void SyncPoint::ClearTrace() {
    impl_->ClearTrace();
}

void SyncPoint::Process(std::string_view point, void* cb_arg) {
    impl_->Process(point, cb_arg);
}

#endif  // NDEBUG
