#include<string>
#include<memory>
#include<initializer_list>

class StrVec {
	friend bool operator==(const StrVec&, const StrVec&);
	friend bool operator!=(const StrVec&, const StrVec&);
	friend bool operator<(const StrVec&, const StrVec&);
public:
	StrVec() :elements(nullptr), first_free(nullptr), cap(nullptr) {}
	StrVec(std::initializer_list < std::string> il);
	StrVec(const StrVec&);
	StrVec& operator=(const StrVec&);
	std::string& operator[](size_t n) { return *(elements + n); }
	const std::string& operator[](size_t n) const { return elements[n]; }
	~StrVec();
	void push_back(const std::string&);
	size_t size() const { return first_free - elements; }
	size_t capacity() const { return cap - elements; }
	std::string* begin() const { return elements; }
	std::string* end() const { return first_free; }

	void reserve(size_t n);
	void resize(size_t n);
	void resize(size_t n, const std::string& s);

private:
	static std::allocator<std::string> alloc;
	void chk_n_alloc() { if (size() == capacity()) reallocate(); }

	std::pair<std::string*, std::string*> alloc_n_copy(const std::string*, const std::string*);
	void free();
	void reallocate();
	std::string* elements;
	std::string* first_free;
	std::string* cap;
};

std::allocator<std::string> StrVec::alloc;
void StrVec::push_back(const std::string& s)
{
	chk_n_alloc();
	alloc.construct(first_free++, s);
}

std::pair<std::string*, std::string*> StrVec::alloc_n_copy(const std::string* b, const std::string* e)
{
	auto data = alloc.allocate(e - b);
	return{ data, uninitialized_copy(b, e, data) };
}

void StrVec::free()
{
	if (elements) {
		for (auto p = first_free; p != elements;) {
			alloc.destroy(--p);
		}
		alloc.deallocate(elements, cap - elements);
	}
}

StrVec::StrVec(const StrVec& s)
{
	auto newdata = alloc_n_copy(s.begin(), s.end());
	elements = newdata.first;
	first_free = cap = newdata.second;
}

StrVec::StrVec(std::initializer_list <std::string> il)
{
	auto data = alloc_n_copy(il.begin(), il.end());
	elements = data.first;
	first_free = cap = data.second;

}

StrVec::~StrVec()
{
	free();
}

StrVec& StrVec::operator=(const StrVec& rhs)
{
	auto data = alloc_n_copy(rhs.begin(), rhs.end());
	free();
	elements = data.first;
	first_free = cap = data.second;
	return *this;
}

void StrVec::reallocate()
{
	auto newcapacity = size() ? 2 * size() : 1;
	auto newdata = alloc.allocate(newcapacity);
	auto dest = newdata;
	auto elem = elements;

	for (size_t i = 0; i != size(); ++i) {
		alloc.construct(dest++, std::move(*elem++));
	}

	free();
	elements = newdata;
	first_free = dest;
	cap = elements + newcapacity;

}

void StrVec::reserve(size_t n)
{
	if (n > capacity()) {
		auto new_n = alloc.allocate(n);
		auto dest = new_n;
		auto elem = elements;

		for (size_t i = 0; i != size(); ++i) {
			alloc.construct(dest++, std::move(*elem++));
		}

		free();
		elements = new_n;
		first_free = dest;
		cap = elements + n;
	}
}

void StrVec::resize(size_t n)
{
	resize(n, std::string());
}

void StrVec::resize(size_t n, const std::string& s)
{
	if (n > size()) {
		if (n > capacity()) { reserve(n * 2); }
		for (size_t i = size(); i != n; ++i) {
			alloc.construct(first_free++, std::move(s));
		}
	}
	else if (n < size()) {
		while (first_free != elements + n) {
			alloc.destroy(--first_free);
		}
	}
}

bool operator==(const StrVec& s1, const StrVec& s2)
{
	if (s1.size() == s2.size()) {
		auto b_ptr1 = s1.begin();
		auto b_ptr2 = s2.begin();
		while (b_ptr1 != s1.end()) {
			if (*b_ptr1 != *b_ptr2) {
				return false;
			}
			else {
				++b_ptr1;
				++b_ptr2;
			}
		}
		return true;
	}
	else {
		return false;
	}
}
bool operator!=(const StrVec& s1, const StrVec& s2)
{
	return !(s1 == s2);
}

bool operator<(const StrVec& s1, const StrVec& s2)
{
	if (s1.size() < s2.size()) {
		return true;
	}
	else if (s1.size() > s2.size()) {
		return false;
	}
	else {
		auto ptr1 = s1.begin();
		auto ptr2 = s2.begin();
		while (ptr1 != s1.end()) {
			if (*ptr1 < *ptr2) {
				return true;
			}
			else if (*ptr1 > *ptr2) {
				return false;
			}
			else {
				++ptr1;
				++ptr2;
			}
		}
		return false;
	}
}