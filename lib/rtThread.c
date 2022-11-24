#include <inc/rtThread.h>
#define DEVICE_NUM 10

static struct rt_device device_list[DEVICE_NUM];
static u32 nres = 0; //资源数
#define NUMBER_OF_CUSTOMERS 2
#define NUMBER_OF_RESOURCES 2                                    //使用两个
static int all_devices[NUMBER_OF_RESOURCES];                     //设备数量数组
static int available[NUMBER_OF_RESOURCES];                       //剩余可用设备数量
static int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];    //进程要求的最大数量
static int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES]; //已经分配给进程的数量
static int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];       //进程还需要的数量
static int status[NUMBER_OF_CUSTOMERS];                          //进程是否完成 1 为完成
static int asid_list[NUMBER_OF_CUSTOMERS];                       // asid to index mapping

extern u32 get_asid();

void asid_list_init()
{
    u32 i;
    for (i = 0; i < NUMBER_OF_CUSTOMERS; i++)
        asid_list[i] = -1;
}

int getAsidIndex()
{

    int temp = 0xff & get_asid();

    int i = 0;
    for (i; i < NUMBER_OF_CUSTOMERS; i++)
    {
        if (asid_list[i] == temp)
            return i;
    }
    for (i = 0; i < NUMBER_OF_CUSTOMERS; i++)
    { //如果没有，则注册到List中，并返回序列号
        if (asid_list[i] == -1)
        {
            asid_list[i] = temp;
            return i;
        }
    }
    return -1;
}

//申请资源
bool rt_require_device(u32 device_id, u32 request_num)
{
    if (device_list[device_id].rt_require_device == NULL)
    {
        //printf("return from rt_require");
        return false;
    }
    int customer_num = getAsidIndex();
    if (customer_num == -1)
        return false;
    int result = 1;
    int status_tmp[NUMBER_OF_CUSTOMERS];
    int available_after_assign[NUMBER_OF_RESOURCES];
    int tmp[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
    int allocation_after_assign[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

    for (u32 d = 0; d < NUMBER_OF_CUSTOMERS; d++)
        status_tmp[d] = status[d];
    for (u32 n = 0; n < NUMBER_OF_RESOURCES; n++)
    {
        printf("available before:%d    ", available[n]);
        available_after_assign[n] = available[n]; 
        printf("\n");
    }
    for (u32 c = 0; c < NUMBER_OF_CUSTOMERS; c++)
    {
        for (u32 d = 0; d < NUMBER_OF_RESOURCES; d++)
        {
            tmp[c][d] = need[c][d];    
            allocation_after_assign[c][d] = allocation[c][d];    
        }
        printf("\n");
    }


    //以上代码为复制当前的状态
    bool sum = true;

    if (!(request_num <= tmp[customer_num][device_id] && request_num <= available_after_assign[device_id]))
        sum = false;


    if (sum)
    {
        available_after_assign[device_id] -= request_num;          //模拟这个进程结束
        allocation_after_assign[customer_num][device_id] += request_num;
        tmp[customer_num][device_id] -= request_num;
    
    }
    u32 ptr = 0;
    u32 fail = 0;
    //以上代码为预处理申请，后面的while循环检查分配后是否是安全状态

    for (u32 n = 0; n < NUMBER_OF_RESOURCES; n++)
        printf("%d    ", available_after_assign[n]);
    printf("\n");

    while (1) {
        int flag = 0;
        int count = 0;
        for (ptr = 0; ptr < NUMBER_OF_CUSTOMERS; ptr++) {
            int mark = 1;
            if (status_tmp[ptr] != 1) {
                for (u32 n = 0; n < NUMBER_OF_RESOURCES; n++) {
                    printf("%d available after assign:%d, tmp:%d \n", n, available_after_assign[n], tmp[ptr][n]);
                    if(tmp[ptr][n] > available_after_assign[n]) {
                        mark = 0;
                        break;
                    }
                }
                if(mark == 1) {
                    flag = 1;
                    break;
                }
            } else {
                count++;
            } 
        }

        if (count >= NUMBER_OF_CUSTOMERS)
            goto assign;

        if (flag == 1) {
            for (u32 n = 0; n < NUMBER_OF_RESOURCES; n++)
                available_after_assign[n] += allocation_after_assign[ptr][n]; 
            status_tmp[ptr] = 1;
        } else {
            result = 0;
            goto exit;
        }
    }


//分配后还是安全的，则执行资源分配
assign:

    need[customer_num][device_id] -= request_num;
    allocation[customer_num][device_id] += request_num;
    available[device_id] -= request_num;

    
//分配后不安全，则跳过资源分配部分，直接释放信号量
exit:

    for (u32 n = 0; n < NUMBER_OF_RESOURCES; n++)
        printf("%d ", request_num);
    printf("from %d ", customer_num);
    if (result)
    {
        printf("fullfilled\n");
        device_list[device_id].rt_require_device(request_num);            //给该进程分配相应数目资源
    }
    else {
        printf("denied\n");
    }

    return result;
}

bool rt_release_device(u32 device_id, u32 request_num)
{
    //释放拥有的资源
    if (device_list[device_id].rt_release_device == NULL)
        return false;
    device_list[device_id].rt_release_device(request_num);
}

bool rt_device_init(u32 device_id, bool (*rt_device_read)(char *), bool (*rt_device_write)(char *), bool (*rt_device_write_byte)(char *, u32), u32 num, bool (*rt_device_write_by_num)(char *, u32 i), bool (*rt_device_read_by_num)(char *, u32 i), bool (*rt_require_device)(u32 i), bool (*rt_release_device)(u32 i))
{
    device_list[device_id].rt_device_read = rt_device_read;
    device_list[device_id].rt_device_write = rt_device_write;
    device_list[device_id].rt_device_write_byte = rt_device_write_byte;
    device_list[device_id].device_num = num;
    device_list[device_id].rt_device_write_by_num = rt_device_write_by_num;
    device_list[device_id].rt_device_read_by_num = rt_device_read_by_num;
    device_list[device_id].rt_release_device = rt_release_device;
    device_list[device_id].rt_require_device = rt_require_device;
    if (device_id < NUMBER_OF_RESOURCES)
    { //仅对前几个设备进行管理
        available[device_id] = num;
        all_devices[device_id] = num;
        printf("%d device has %d items\n", device_id, num);
    }
    nres++;

    return true;
}

bool rt_device_write(u32 device_id, char *buf)
{
    if (device_list[device_id].rt_device_write == NULL)
        return false;
    return device_list[device_id].rt_device_write(buf);
}

bool rt_device_write_byte(u32 device_id, char *buf, u32 i)
{
    if (device_list[device_id].rt_device_write == NULL)
        return false;
    return device_list[device_id].rt_device_write_byte(buf, i);
}

bool rt_device_read(u32 device_id, char *buf)
{
    if (device_list[device_id].rt_device_read == NULL)
        return false;
    return device_list[device_id].rt_device_read(buf);
}

//声明该进程需要的最大资源数量
bool rt_claim_device(u32 *require)
{
    int index = getAsidIndex();
    printf("%d", index);
                               //进程还需要的数量
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        need[index][i] = require[i];
        maximum[index][i] = require[i];
        allocation[index][i] = 0;
    }
    status[index] = 0;
}

bool rt_device_write_by_num(u32 device_id, u32 num, char *buf)
{
    if (device_list[device_id].rt_device_write_by_num == NULL)
        return false;
    return device_list[device_id].rt_device_write_by_num(buf, num);
}

bool rt_device_read_by_num(u32 device_id, u32 num, char *buf)
{
    if (device_list[device_id].rt_device_read_by_num == NULL)
        return false;
    return device_list[device_id].rt_device_read_by_num(buf, num);
}

bool rt_task_exit()
{
    int index = getAsidIndex();
    asid_list[index] = -1;
    return true;
}
