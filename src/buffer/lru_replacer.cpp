//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_replacer.h"

namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) { this->capacity_ = num_pages; }

LRUReplacer::~LRUReplacer() = default;

bool LRUReplacer::Victim(frame_id_t *frame_id) {
  std::lock_guard lock(lru_latch_);
  if (lru_list_.empty()) {
    return false;
  }
  auto &lru = lru_list_.front();
  *frame_id = lru;
  cache_.erase(lru);
  lru_list_.pop_front();
  return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
  std::lock_guard lock(lru_latch_);
  if (cache_.find(frame_id) != cache_.end()) {
    // if the frame is not in the cache, add it to the cache
    lru_list_.erase(cache_[frame_id]);
    cache_.erase(frame_id);
  }
  assert(cache_.size() == lru_list_.size());
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  std::lock_guard lock(lru_latch_);
  if (cache_.find(frame_id) == cache_.end()) {
    if (lru_list_.size() == capacity_) {
      // if size is full, call victim to remove the last element
      Victim(&frame_id);
    }
    lru_list_.push_back(frame_id);
    cache_[frame_id] = --lru_list_.end();  // last element is the most recently used
  }
  assert(cache_.size() == lru_list_.size());
}

size_t LRUReplacer::Size() { return cache_.size(); }

}  // namespace bustub
