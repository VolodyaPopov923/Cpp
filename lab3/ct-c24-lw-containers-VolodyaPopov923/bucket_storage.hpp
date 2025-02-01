#ifndef BUCKET_STORAGE
#define BUCKET_STORAGE

#include <expected>
#include <iostream>
#include <iterator>

template< typename T >
class iterator;

template< typename T >
class const_iterator;

template< typename T >
class BucketStorage
{
  public:
	using value_type = T;
	using pointer = T *;
	using reference = T &;
	using const_reference = const T &;
	using difference_type = long;
	using size_type = size_t;
	using iterator = iterator< T >;
	using const_iterator = const_iterator< T >;
	friend iterator;
	friend const_iterator;

  public:
	BucketStorage(BucketStorage const &other);
	BucketStorage(BucketStorage &&other) noexcept;
	explicit BucketStorage(size_t block_capacity = 64);

	iterator insert(const T &value);
	iterator insert(T &&value);

	iterator erase(const_iterator it) noexcept;

	bool empty() const noexcept;

	size_t size() const noexcept;
	size_t capacity() const noexcept;
	void shrink_to_fit();
	void clear();
	void swap(BucketStorage &other) noexcept;
	iterator begin() noexcept;
	const_iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;
	iterator end() noexcept;
	const_iterator end() const noexcept;
	const_iterator cend() const noexcept;

	iterator get_to_distance(iterator it, difference_type distance);

	BucketStorage< T > &operator=(const BucketStorage< T > &other);
	BucketStorage< T > &operator=(BucketStorage< T > &&other) noexcept;
	~BucketStorage();

  private:
	class Block
	{
	  public:
		explicit Block(size_t capacity = 64, Block *bwd = nullptr, size_t block_num = 0);
		Block(const Block &other);
		~Block();
		const size_t block_number;
		const size_t capacity;
		Block *bwd;
		Block *fwd;
		size_t size;
		size_t *deleted;
		size_t deleted_count;
		size_t *activity;
		T *data;
		iterator insert();
		size_t erase(size_t i);
		size_t head;
		size_t tail;
		size_t next(size_t current) const;
		size_t previous(size_t current) const;
		bool isActive(size_t i);
	};
	Block *head;
	Block *tail;
	size_type n;
	size_type elements;
	size_type block_capacity;
};

template< typename T >
class iterator
{
  public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = T;
	using pointer_type = T *;
	using reference_type = T &;
	using difference_type = long;
	friend class const_iterator< T >;
	friend class BucketStorage< T >;

	iterator< T >(size_t i, typename BucketStorage< T >::Block *current);
	explicit iterator< T >(const const_iterator< T > &other);

	iterator< T > &operator++();
	iterator< T > operator++(int);
	iterator< T > &operator--();
	iterator< T > operator--(int);

	reference_type operator*();
	pointer_type operator->() const;

	bool operator==(const iterator< T > &a) const;
	bool operator==(const const_iterator< T > &a) const;
	bool operator!=(const iterator< T > &a) const;
	bool operator<(const iterator< T > &a) const;
	bool operator<=(const iterator< T > &a) const;
	bool operator>(const iterator< T > &a) const;

	bool operator>=(const iterator< T > &a) const;
	iterator< T > &operator=(iterator< T > a);

  private:
	typename BucketStorage< T >::Block *block;
	size_t i;
};

template< typename T >
class const_iterator
{
  public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = const T;
	using pointer_type = const T *;
	using reference_type = const T &;
	using difference_type = long;
	friend class BucketStorage< T >;
	friend class iterator< T >;

	const_iterator< T >(size_t i, typename BucketStorage< T >::Block *current);
	const_iterator< T >(const iterator< T > &other);

	const_iterator< T > &operator++();
	const_iterator< T > operator++(int);
	const_iterator< T > &operator--();
	const_iterator< T > operator--(int);

	value_type operator*();
	reference_type operator->() const;

	bool operator==(const const_iterator< T > &a) const;
	bool operator!=(const const_iterator< T > &a) const;
	bool operator<(const const_iterator< T > &a) const;
	bool operator<=(const const_iterator< T > &a) const;
	bool operator>(const const_iterator< T > &a) const;

	bool operator>=(const const_iterator< T > &a) const;
	const_iterator< T > &operator=(const_iterator< T > a);

  private:
	typename BucketStorage< T >::Block *block;
	size_t i;
};

template< typename T >
BucketStorage< T >::BucketStorage(const BucketStorage &other) :
	n(other.n), elements(other.elements), block_capacity(other.block_capacity)
{
	this->head = new Block(*other.head);
	Block *current = this->head;
	while (current->fwd != nullptr)
		current = current->fwd;
	this->tail = current;
}
template< typename T >
BucketStorage< T >::BucketStorage(BucketStorage &&other) noexcept :
	head(other.head), tail(other.tail), n(other.n), elements(other.elements), block_capacity(other.block_capacity)
{
	swap(other);
	other.head = nullptr;
	other.tail = nullptr;
}
template< typename T >
BucketStorage< T >::BucketStorage(const size_t block_capacity) :
	elements(0), head(nullptr), tail(nullptr), n(0), block_capacity(block_capacity)
{
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::insert(const T &value)
{
	if (n == 0)
	{
		head = new Block(block_capacity, nullptr, 0);
		tail = head;
		n++;
	}
	iterator it = head->insert();
	if (it.block->bwd == tail)
	{
		tail = it.block;
		n++;
	}
	it.block->data[it.i] = value;
	elements++;
	return it;
}
template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::insert(T &&value)
{
	if (n == 0)
	{
		head = new Block(block_capacity, nullptr, 0);
		tail = head;
		n++;
	}
	iterator it = head->insert();
	if (it.block->bwd == tail)
	{
		tail = it.block;
		n++;
	}

	std::memcpy(it.block->data + it.i, &value, sizeof(T));
	elements++;
	return it;
}
template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::erase(const_iterator it) noexcept
{
	elements--;
	size_t next_id = it.block->erase(it.i);
	Block *next_block = it.block;
	if (it.block->size == 0)
	{
		n--;
		if (n == 0)
			return iterator(block_capacity, nullptr);
		if (it.block == tail)
		{
			tail = it.block->bwd;
		}
		if (it.block == head)
		{
			head = it.block->fwd;
		}
		next_block = it.block->fwd;
		if (next_block == nullptr)
			next_block = it.block->bwd;
		delete it.block;
	}
	if (next_id == 0)
	{
		if (next_block->fwd != nullptr)
			return iterator(next_block->fwd->head, next_block->fwd);
		return end();
	}
	return iterator(next_id, next_block);
}
template< typename T >
void BucketStorage< T >::clear()
{
	if (head != nullptr)
	{
		while (head->fwd != nullptr)
			head->fwd->~Block();
		head->~Block();
	}
	head = nullptr;
	tail = nullptr;
	elements = 0;
	n = 0;
}

template< typename T >
size_t BucketStorage< T >::capacity() const noexcept
{
	return block_capacity * n;
}
template< typename T >
typename BucketStorage< T >::size_type BucketStorage< T >::size() const noexcept
{
	return elements;
}
template< typename T >
bool BucketStorage< T >::empty() const noexcept
{
	return elements == 0;
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::end() noexcept
{
	return iterator(block_capacity, tail);
}
template< typename T >
typename BucketStorage< T >::const_iterator BucketStorage< T >::end() const noexcept
{
	return const_iterator(tail->tail, tail);
}
template< typename T >
typename BucketStorage< T >::const_iterator BucketStorage< T >::cend() const noexcept
{
	return const_iterator(tail->tail, tail);
}
template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::begin() noexcept
{
	if (elements == 0)
		return end();
	return iterator(head->head, head);
}
template< typename T >
typename BucketStorage< T >::const_iterator BucketStorage< T >::begin() const noexcept
{
	return const_iterator(head->head, head);
}
template< typename T >
typename BucketStorage< T >::const_iterator BucketStorage< T >::cbegin() const noexcept
{
	return const_iterator(head->head, head);
}
template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::get_to_distance(iterator it, const BucketStorage::difference_type distance)
{
	if (distance < 0)
		for (int i = 0; i < -1 * distance; ++i)
			--it;
	else
		for (int i = 0; i < distance; i++)
			++it;
	return it;
}

template< typename T >
void BucketStorage< T >::swap(BucketStorage &other) noexcept
{
	if (&other == this)
		return;
	Block *tmp_first = other.head;
	Block *tmp_last = other.tail;
	size_t tmp_size = other.elements;
	size_t tmp_blocks = other.n;
	size_t tmp_capacity = other.block_capacity;
	other.head = head;
	other.tail = tail;
	other.elements = elements;
	other.n = n;
	other.block_capacity = block_capacity;
	head = tmp_first;
	tail = tmp_last;
	elements = tmp_size;
	n = tmp_blocks;
	block_capacity = tmp_capacity;
}

template< typename T >
void BucketStorage< T >::shrink_to_fit()
{
	size_t c = elements / block_capacity;
	if (c * block_capacity < elements)
		c++;
	while (n > c)
	{
		insert(std::move(tail->data[tail->tail]));
		erase(const_iterator(tail->tail, tail));
	}
}

template< typename T >
BucketStorage< T > &BucketStorage< T >::operator=(const BucketStorage< T > &other)
{
	if (this == &other)
		return *this;

	elements = other.elements;
	n = other.n;
	block_capacity = other.block_capacity;

	this->head = new Block(*other.head);
	Block *current = this->head;
	while (current->fwd != nullptr)
	{
		current = current->fwd;
	}
	this->tail = current;
	return *this;
}

template< typename T >
BucketStorage< T > &BucketStorage< T >::operator=(BucketStorage< T > &&other) noexcept
{
	if (this == &other)
		return *this;
	head = other.head;
	tail = other.tail;
	n = other.n;
	elements = other.elements;
	block_capacity = other.block_capacity;
	other.head = nullptr;
	other.tail = nullptr;
	return *this;
}

template< typename T >
BucketStorage< T >::~BucketStorage()
{
	if (head != nullptr)
	{
		while (head->fwd != nullptr)
			head->fwd->~Block();
		head->~Block();
	}
	head = nullptr;
	tail = nullptr;
	elements = 0;
	n = 0;
}
template< typename T >
BucketStorage< T >::Block::Block(size_t capacity, BucketStorage::Block *bwd, size_t block_number) :
	capacity(capacity), size(0), bwd(bwd), fwd(nullptr), block_number(block_number), head(0), tail(0),
	deleted(new size_t[capacity]), deleted_count(0), activity(new size_t[capacity]),
	data(static_cast< T * >(malloc(sizeof(T) * capacity)))
{
}
template< typename T >
BucketStorage< T >::Block::Block(const BucketStorage< T >::Block &other) :
	size(other.size), capacity(other.capacity), bwd(nullptr), block_number(other.block_number), head(other.head),
	tail(other.tail), deleted(new size_t[capacity]), deleted_count(other.deleted_count), activity(new size_t[capacity]),
	data(static_cast< T * >(malloc(sizeof(T) * capacity)))
{
	std::memcpy(activity, other.activity, sizeof(size_t) * capacity);
	std::memcpy(deleted, other.deleted, sizeof(size_t) * capacity);
	if (other.fwd != nullptr)
		this->fwd = new Block(*fwd);
	this->fwd->bwd = this;
	std::memcpy(data, other.data, sizeof(T) * capacity);
}
template< typename T >
BucketStorage< T >::Block::~Block()
{
	if (bwd != nullptr)
		bwd->fwd = this->fwd;
	if (fwd != nullptr)
		fwd->bwd = this->bwd;
	size_t i = head;
	while (size != 0)
	{
		erase(i);
		i = next(i);
	}
	delete[] deleted;
	delete[] activity;
	free(data);
}
template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::Block::insert()
{
	if (size == 0)
	{
		activity[0] = 0;
		size++;
		return iterator(0, this);
	}
	if (size == capacity)
	{
		if (fwd == nullptr)
			fwd = new Block(capacity, this, block_number + 1);
		return fwd->insert();
	}
	if (deleted_count == 0)
	{
		activity[++tail] = 0;
		size++;
		return iterator(tail, this);
	}
	size++;
	size_t i = deleted[--deleted_count];
	if (i < head)
	{
		activity[i + 1] = head - i - 1;
		activity[head - 1] = activity[i + 1];
		head = i;
		return iterator(i, this);
	}
	if (i >= tail)
	{
		tail++;
		activity[tail] = 0;

		return iterator(tail, this);
	}
	if (isActive(i + 1))
	{
		activity[i - 1] = activity[i] - 1;
		activity[i - activity[i - 1]] = activity[i - 1];
	}
	else if (isActive(i - 1))
	{
		activity[i + 1] = activity[i] - 1;
		activity[i + activity[i + 1]] = activity[i + 1];
	}
	else
	{
		size_t next_id = i + activity[i] - 1;
		size_t prev_id = next_id - activity[next_id] + 1;
		size_t nxt = activity[i] - 1;
		size_t prv = activity[prev_id] - activity[i];
		activity[i + 1] = nxt;
		activity[next_id] = nxt;
		activity[prev_id] = prv;
		activity[i - 1] = prv;
		activity[i] = 0;
	}
	return iterator(i, this);
}
template< typename T >
size_t BucketStorage< T >::Block::erase(size_t i)
{
	data[i].~T();
	size--;
	if (i == tail)
	{
		tail = i - 1 - activity[i - 1];
		return 0;
	}
	deleted[deleted_count++] = i;
	if (i == head)
	{
		head = i + 1 + activity[i + 1];
		return head;
	}
	if (isActive(i + 1))
	{
		activity[i] = activity[i - 1] + 1;
		activity[i - activity[i] + 1] = activity[i];
	}
	else if (isActive(i - 1))
	{
		activity[i] = activity[i + 1] + 1;
		activity[i + activity[i] - 1] = activity[i];
	}
	else
	{
		size_t prv = activity[i - activity[i - 1]] + activity[i + 1] + 1;
		size_t curr = activity[i + 1] + 1;
		size_t nxt = activity[i + activity[i + 1]] + activity[i - 1] + 1;
		size_t prev_id = i - activity[i - 1];
		size_t next_id = i + activity[i + 1];
		activity[i] = curr;
		activity[prev_id] = prv;
		activity[next_id] = nxt;
	}
	return next(i);
}

template< typename T >
size_t BucketStorage< T >::Block::next(size_t current) const
{
	if (current == tail)
		return 0;
	return activity[current + 1] + current + 1;
}

template< typename T >
size_t BucketStorage< T >::Block::previous(size_t current) const
{
	if (current == head)
		return capacity;
	if (current == capacity)
		return tail;
	return current - activity[current - 1] - 1;
}

template< typename T >
bool BucketStorage< T >::Block::isActive(size_t i)
{
	return activity[i] == 0 && i >= head && i <= tail;
}

template< typename T >
iterator< T >::iterator(size_t i, typename BucketStorage< T >::Block *current) : i(i), block(current)
{
}

template< typename T >
iterator< T >::iterator(const const_iterator< T > &other) : iterator(other.i, other.block)
{
}

template< typename T >
iterator< T > &iterator< T >::operator++()
{
	i = block->next(i);
	if (i == 0)
	{
		if (block->fwd != nullptr)
		{
			block = block->fwd;
			i = block->head;
		}
		else
		{
			i = block->capacity;
		}
	}
	return *this;
}
template< typename T >
iterator< T > iterator< T >::operator++(int)
{
	iterator< T > tmp = *this;
	++(*this);
	return tmp;
}
template< typename T >
iterator< T > &iterator< T >::operator--()
{
	i = block->previous(i);
	if (i == block->capacity)
	{
		if (block->bwd != nullptr)
		{
			block = block->bwd;
			i = block->tail;
		}
		else
		{
			i = block->head;
		}
	}
	return *this;
}
template< typename T >
iterator< T > iterator< T >::operator--(int)
{
	iterator< T > tmp = *this;
	--(*this);
	return tmp;
}
template< typename T >
iterator< T >::reference_type iterator< T >::operator*()
{
	return block->data[i];
}
template< typename T >
iterator< T >::pointer_type iterator< T >::operator->() const
{
	return &block->data[i];
}

template< typename T >
bool iterator< T >::operator==(const iterator< T > &a) const
{
	return block == a.block && i == a.i;
}

template< typename T >
bool iterator< T >::operator==(const const_iterator< T > &a) const
{
	return *this == iterator< T >(a);
}
template< typename T >
bool iterator< T >::operator<(const iterator< T > &a) const
{
	return block->block_number < a.block->block_number || block == a.block && i < a.i;
}
template< typename T >
bool iterator< T >::operator<=(const iterator< T > &a) const
{
	return *this < a || *this == a;
}
template< typename T >
bool iterator< T >::operator>=(const iterator< T > &a) const
{
	return *this > a || *this == a;
}

template< typename T >
iterator< T > &iterator< T >::operator=(iterator< T > a)
{
	block = a.block;
	i = a.i;
	return *this;
}
template< typename T >
const_iterator< T >::const_iterator(size_t i, typename BucketStorage< T >::Block *current) : i(i), block(current)
{
}

template< typename T >
const_iterator< T >::const_iterator(const iterator< T > &other) : const_iterator(other.i, other.block)
{
}

template< typename T >
const_iterator< T > &const_iterator< T >::operator++()
{
	i = block->next(i);
	if (i == 0)
	{
		if (block->fwd != nullptr)
		{
			block = block->fwd;
			i = block->head;
		}
		else
		{
			i = block->capacity;
		}
	}
	return *this;
}
template< typename T >
const_iterator< T > const_iterator< T >::operator++(int)
{
	const_iterator< T > tmp = *this;
	++(*this);
	return tmp;
}
template< typename T >
const_iterator< T > &const_iterator< T >::operator--()
{
	i = block->previous(i);
	if (i == block->capacity)
	{
		if (block->bwd != nullptr)
		{
			block = block->bwd;
			i = block->tail;
		}
		else
		{
			i = block->begin;
		}
	}
	return this;
}
template< typename T >
const_iterator< T > const_iterator< T >::operator--(int)
{
	const_iterator< T > tmp = *this;
	*this --;
	return tmp;
}
template< typename T >
const_iterator< T >::value_type const_iterator< T >::operator*()
{
	return block->data[i];
}
template< typename T >
const_iterator< T >::reference_type const_iterator< T >::operator->() const
{
	return block->data[i];
}

template< typename T >
bool const_iterator< T >::operator==(const const_iterator< T > &a) const
{
	return block == a.block && i = a.i;
}

template< typename T >
bool const_iterator< T >::operator<(const const_iterator< T > &a) const
{
	return block->block_number < a.block->block_number || block == a.block && i < a.i;
}
template< typename T >
bool const_iterator< T >::operator<=(const const_iterator< T > &a) const
{
	return *this < a || *this == a;
}

template< typename T >
bool const_iterator< T >::operator>=(const const_iterator< T > &a) const
{
	return *this > a || *this == a;
}

template< typename T >
const_iterator< T > &const_iterator< T >::operator=(const_iterator< T > a)
{
	block = a.block;
	i = a.i;
	return *this;
}

#endif
