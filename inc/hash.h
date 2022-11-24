//共享内存 Hash表
#include<stddef.h>
#define HashMaxSize 10    //宏定义哈希表的最大容量
#define LoadFactor 0.8       //宏定义负载因子，用于表示哈希表的负载能力。



typedef int KeyType;         
typedef struct Page* ValueType;
typedef size_t (*HashFunc)(KeyType key);     //重定义哈希函数


typedef enum elm_State     //用于表示每个元素的状态
{
    Empty,     //空，当前没有值
    Valid,     //有效，当前的值有效
    Invalid    //非空但无效，表示当前结点被删除
}elm_State;

typedef struct HashElem      //哈希表的元素结构体
{
    KeyType key;
    ValueType value;
    elm_State stat;
}HashElem;

typedef struct HashTable              
{
    HashElem data[HashMaxSize];
    size_t size;                 //当前有效的元素个数
    HashFunc hashfunc;
}HashTable;


size_t tryHashFuncDefault(KeyType key);

void tryHashTableInit(HashTable *ht);
//哈希表的插入 ,插入成功返回1，插入失败返回0
int tryHashTableInsert(HashTable* ht, KeyType key, ValueType value);
//哈希表的查找 找到返回1，没找到返回0
ValueType tryHashTableFind(HashTable* ht, KeyType key, ValueType value);
//删除节点
int tryHashTableFindCur(HashTable* ht, KeyType key, size_t* cur);
void tryHashRemove(HashTable* ht, KeyType key);

int tryHashEmpty(HashTable* ht);

//求哈希表的大小
size_t tryHashSize(HashTable* ht);

void tryHashPrint(HashTable* ht,const char* msg);

