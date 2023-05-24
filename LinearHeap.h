#ifndef _LINEAR_HEAP_H_
#define _LINEAR_HEAP_H_

#include "Utility.h"

class ListLinearHeap {
private:
	ui n; // number vertices
	ui key_cap; // the maximum allowed key value

	ui min_key; // possible min key
	ui max_key; // possible max key

	ui *key_s; // key of vertices

	ui *head_s; // head of doubly-linked list for a specific weight

	ui *pre_s; // pre for doubly-linked list
	ui *next_s; // next for doubly-linked list
	long long int max_occupy=0;

public:
	ListLinearHeap(ui _n, ui _key_cap) {
		n = _n;
		key_cap = _key_cap;

		min_key = max_key = key_cap;

		head_s = key_s = pre_s = next_s = nullptr;
	}

	~ListLinearHeap() {
		if(head_s != nullptr) {
			delete[] head_s;
			head_s = nullptr;
		}
		if(pre_s != nullptr) {
			delete[] pre_s;
			pre_s = nullptr;
		}
		if(next_s != nullptr) {
			delete[] next_s;
			next_s = nullptr;
		}
		if(key_s != nullptr) {
			delete[] key_s;
			key_s = nullptr;
		}
	}
	long long get_max_occupy(){
		return max_occupy;
	}
	// 线性堆初始化，_n为总数，_key_cap为最大值，id_s为下标i对应的点，key_s[i]为点i的值
	void init(ui _n, ui _key_cap, ui *_id_s, ui *_key_s) {
		if(key_s == nullptr) key_s = new ui[n];
		if(pre_s == nullptr) pre_s = new ui[n];
		if(next_s == nullptr) next_s = new ui[n];
		if(head_s == nullptr) head_s = new ui[key_cap+1];
		max_occupy+=sizeof(int)*(n+n+n+key_cap+1);

		assert(_key_cap <= key_cap);
		min_key = max_key = _key_cap;
		for(ui i = 0;i <= _key_cap;i ++) head_s[i] = n;

		for(ui i = 0;i < _n;i ++) {
			ui id = _id_s[i];
			ui key = _key_s[id];
			if(key>_key_cap) 
			assert(id < n); assert(key <= _key_cap);

			key_s[id] = key; pre_s[id] = n; next_s[id] = head_s[key];
			if(head_s[key] != n) pre_s[head_s[key]] = id;
			head_s[key] = id;

			if(key < min_key) min_key = key;
		}
	}

	ui get_key(ui id) { return key_s[id]; }
	// 从小到大存入vs中
	void get_ids(ui *vs, ui &vs_size) {
		for(ui i = min_key;i <= max_key;i ++) {
			for(ui id = head_s[i];id != n;id = next_s[id]) {
				vs[vs_size ++] = id;
			}
		}
	}
	// 获取堆中最小值，id为最小值的点，key为最小值
	bool get_min(ui &id, ui &key) {// return true if success, return false otherwise
		while(min_key <= max_key&&head_s[min_key] == n) ++ min_key;
		if(min_key > max_key) return false;

		id = head_s[min_key];
		key = min_key;

		assert(key_s[id] == key);

		return true;
	}
	// 获取最小值+删除最小值
	bool pop_min(ui &id, ui &key) {// return true if success, return false otherwise
		while(min_key <= max_key&&head_s[min_key] == n) ++ min_key;
		if(min_key > max_key) return false;

		id = head_s[min_key];
		key = min_key;

		assert(key_s[id] == key);

		head_s[min_key] = next_s[id];
		if(head_s[min_key] != n) pre_s[head_s[min_key]] = n;
		return true;
	}
	// 更新点id的值，值减去dec
	ui decrement(ui id, ui dec) {
		if(key_s[id] < dec){
			printf("key_s[%d]=%d,dec=%d\n", id, key_s[id], dec);
		}
		assert(key_s[id] >= dec);

		if(pre_s[id] == n) {
			assert(head_s[key_s[id]] == id);
			head_s[key_s[id]] = next_s[id];
			if(next_s[id] != n) pre_s[next_s[id]] = n;
		}
		else {
			ui pid = pre_s[id];
			next_s[pid] = next_s[id];
			if(next_s[id] != n) pre_s[next_s[id]] = pid;
		}

		ui &key = key_s[id];
		key -= dec; pre_s[id] = n; next_s[id] = head_s[key];
		if(head_s[key] != n) pre_s[head_s[key]] = id;
		head_s[key] = id;

		if(key < min_key) min_key = key;
		return key;
	}
};

#endif
