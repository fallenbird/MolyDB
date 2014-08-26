#ifndef __LOCK_FREE_QUEUE_HPP__
#define __LOCK_FREE_QUEUE_HPP__

#include <stddef.h>

struct node_t;
struct pointer_t 
{
	node_t *ptr;
	unsigned int tag;
	pointer_t() 
	{
		ptr = NULL;
		tag = 0;
	}
	pointer_t(node_t *a_ptr, unsigned int a_tag)
	{
		ptr = a_ptr; tag=a_tag;
	}

	friend bool operator==(pointer_t const &l, pointer_t const &r)
	{
		return l.ptr == r.ptr && l.tag == r.tag;
	}

	friend bool operator!=(pointer_t const &l, pointer_t const &r)
	{
		return !(l == r);
	}
};

typedef void * data_type;

#define dummy_val NULL

struct node_t 
{ 
	pointer_t next; // wgg 发现了64位错误的原因是 cmp16b需要16字节对齐，现在改正过来了。
	data_type value; 
	node_t()
	{
		value = dummy_val;
		next=  pointer_t(NULL,0);
	}
};



#ifdef __x86_64__
inline bool CAS2(pointer_t *addr, pointer_t &old_value, pointer_t &new_value)
{
	bool  ret;
	//__asm__ __volatile__(
	//	"lock cmpxchg16b %1;\n"
	//	"sete %0;\n"
	//	:"=m"(ret),"+m" (*(volatile pointer_t *) (addr))
	//	:"a" (old_value.ptr), "d" (old_value.tag), "b" (new_value.ptr), "c" (new_value.tag));
	//return ret;


	//__asm
	//{
	//	"lock cmpxchg16b %1;\n"
	//	"sete %0;\n"
	//	:"=m"(ret),"+m" (*(volatile pointer_t *) (addr))
	//	:"a" (old_value.ptr), "d" (old_value.tag), "b" (new_value.ptr), "c" (new_value.tag)
	//}
	return ret;
}

#else
inline bool CAS2(pointer_t *addr, pointer_t &old_value, pointer_t &new_value)
{
	bool  ret;
	//__asm__ __volatile__(
	//	"lock cmpxchg8b %1;\n"
	//	"sete %0;\n"
	//	:"=m"(ret),"+m" (*(volatile pointer_t *) (addr))
	//	:"a" (old_value.ptr), "d" (old_value.tag), "b" (new_value.ptr), "c" (new_value.tag));
	//return ret;

	//__asm 
	//{
	//	"lock cmpxchg8b %1;\n"
	//	"sete %0;\n"
	//	:"=m"(ret),"+m" (*(volatile pointer_t *) (addr))
	//	:"a" (old_value.ptr), "d" (old_value.tag), "b" (new_value.ptr), "c" (new_value.tag)
	//}
	return ret;
}
#endif

class JK_LockFreeQueue 
{
	pointer_t tail_;
	pointer_t head_;

public:
	JK_LockFreeQueue() 
	{

	}


	void init() 
	{
		node_t *nd = new node_t();
		nd->next = pointer_t(NULL, 0);
		head_ = pointer_t(nd, 0);
		tail_ = pointer_t(nd, 0);
	}


	void enqueue(data_type val) 
	{
		pointer_t tail, next;
		node_t* nd = new node_t();
		nd->value = val;
		while(true)
		{
			tail = this->tail_;
			next = tail.ptr->next;
			if (tail == this->tail_)
			{
				if(next.ptr == NULL)
				{
					pointer_t new_pt(nd, next.tag+1);
					if(CAS2(&(this->tail_.ptr->next), next, new_pt))
					{
						break; // Enqueue done!
					}
				}
				else 
				{
					pointer_t new_pt(next.ptr, tail.tag+1);
					CAS2(&(this->tail_), tail, new_pt);
				}
			}
		}
		pointer_t new_pt(nd, tail.tag+1);
		CAS2(&(this->tail_), tail, new_pt);
	}


	data_type dequeue()
	{
		pointer_t tail, head, next;
		data_type val=NULL;
		while(true)
		{
			head = this->head_;
			tail = this->tail_;
			next = (head.ptr)->next;
			if (head != this->head_) continue;

			if(head.ptr == tail.ptr)
			{
				if (next.ptr == NULL)
				{
					return NULL;
				}
				pointer_t new_pt(next.ptr, tail.tag+1);
				CAS2(&(this->tail_), tail, new_pt);
			} 
			else
			{ 
				val = next.ptr->value;
				pointer_t new_pt(next.ptr, head.tag+1);
				if(CAS2(&(this->head_), head, new_pt))
				{
					break;
				}
			}
		}
		delete head.ptr;
		return val;
	}
};


#endif //__LOCK_FREE_QUEUE_HPP__
