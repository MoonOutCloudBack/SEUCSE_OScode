//共享内存 Hash表

#include <hash.h>


size_t tryHashFuncDefault(KeyType key)
{
    return key % HashMaxSize;
}


void tryHashTableInit(HashTable *ht)
{
    if (ht == NULL)    //非法输入
        return;
    ht->size = 0;
    ht->hashfunc = tryHashFuncDefault;
    for (size_t i = 0; i < HashMaxSize; i++)
    {
        ht->data[i].key = 0;
        ht->data[i].stat = Empty;
        ht->data[i].value = 0;
    }
}
//哈希表的插入 ,插入成功返回1，插入失败返回0
int tryHashTableInsert(HashTable* ht, KeyType key, ValueType value)
{
    if (ht == NULL)
        return 0;
    //先根据哈希函数将key转换，求得key在哈希表中的下标
    size_t cur = ht->hashfunc(key);
    //判断当前下标是否被占用
    while (1)
    {
        if (ht->data[cur].key == key && ht->data[cur].value == value)     //用于保证不会用重复的数字存入哈希表
            return 0;
        if (ht->data[cur].stat != Valid)
        {
            ht->data[cur].key = key;
            ht->data[cur].value = value;
            ht->data[cur].stat = Valid;
            ht->size++;
            return 1;
        }
        cur++;
    }
}

//哈希表的查找 找到返回1，没找到返回0
ValueType tryHashTableFind(HashTable* ht, KeyType key, ValueType value)
{
    if (ht == NULL)
        return NULL;
    //通过哈希函数找到key所对应的下标
    size_t offset = ht->hashfunc(key);
    //若当前下标所对应的值正好是key并且当前的状态必须为valid才返回
    if (ht->data[offset].key == key&&ht->data[offset].stat==Valid)
    {
        value = ht->data[offset].value;
        return value;
    }
    //若当前下标所对应的值不是key，则继续向后进行查找，直到找到stat等于empty
    else
    {
        while (ht->data[offset].stat != Empty)
        {
            if (ht->data[offset].key != key)
            {
                offset++;
                //判断是否下标已超出最大值
                if (offset >= HashMaxSize)
                    offset = NULL;
            }
            else
            {
                if (ht->data[offset].stat == Valid)
                {
                    value = ht->data[offset].value;
                    return value;
                }
                else
                    offset++;
            }
        }
            return NULL;
    }
}
//删除节点
int tryHashTableFindCur(HashTable* ht, KeyType key, size_t* cur)
{
    if (ht == NULL)
        return 0;
    for (size_t i = 0; i < HashMaxSize; i++)
    {
        if (ht->data[i].key == key && ht->data[i].stat == Valid)
        {
            *cur = i;
            return 1;
        }
    }
    return 0;
}
void tryHashRemove(HashTable* ht, KeyType key)
{
    if (ht == NULL)  //非法输入
        return;
    //先用find函数查找key是否存在
    ValueType value = 0;
    size_t cur = 0;                      //得到要删除元素的下标
    int ret=tryHashTableFindCur(ht,key,&cur);//通过find函数得到key是否存在在哈希表中
    if (ret == 0)
        return;
    else
    {
        ht->data[cur].stat = Invalid;
        ht->size--;
    }
}

int tryHashEmpty(HashTable* ht)
{
    if (ht == NULL)
        return 0;
    else
        return ht->size > 0 ? 1 : 0;
}

//求哈希表的大小
size_t tryHashSize(HashTable* ht)
{
    if (ht == NULL)
        return 0;
    return ht->size;
}

void tryHashPrint(HashTable* ht,const char* msg)         //打印哈希表
{
    if (ht == NULL || ht->size == 0)
        return;
    printf("%s\n", msg);
    for (size_t i = 0; i < HashMaxSize; i++)
    {
        if (ht->data[i].stat != Empty)
            printf("[%d]  key=%d  value=%d  stat=%d\n", i, ht->data[i].key,
            ht->data[i].value, ht->data[i].stat);
    }
}

