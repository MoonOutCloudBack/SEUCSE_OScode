#ifndef _rtthread_h_
#define _rtthread_h_
#include <types.h>
#include <printf.h>

struct rt_device
{
    //struct rt_object          parent;        /* 内核对象基类 */
    //enum rt_device_class_type type;          /* 设备类型 */
    uint16_t               flag;          /* 设备参数 */
    uint16_t               open_flag;     /* 设备打开标志 */
    uint8_t                ref_count;     /* 设备被引用次数 */
    uint8_t                device_id;     /* 设备 ID,0 - 255 */
    uint8_t                device_num;    /* 设备数量*/

    //rt_device_read;
    bool (*rt_device_read)(char *);
    //rt_device_write;
    bool (*rt_device_write)(char *);

    bool (*rt_device_write_byte)(char *,u32 i);

    bool (*rt_device_write_by_num)(char *, u32 i);

    bool (*rt_device_read_by_num)(char *, u32 i);

    bool (*rt_require_device)(u32 i);

    bool (*rt_release_device)(u32 i);

    /* 数据收发回调函数 */
    //rt_err_t (*rx_indicate)(rt_device_t dev, rt_size_t size);
    //rt_err_t (*tx_complete)(rt_device_t dev, void *buffer);

    //const struct rt_device_ops *ops;    /* 设备操作方法 */

    /* 设备的私有数据 */
    //void *user_data;
};
//typedef struct rt_device *rt_device_t;

bool rt_device_init(u32 device_id, bool (*rt_device_read)(char *), \
    bool (*rt_device_write)(char *), bool (*rt_device_write_byte)(char *,u32), \
    u32 num, bool (*rt_device_write_by_num)(char *, u32 i),bool (*rt_device_read_by_num)(char *, u32 i), \
    bool (*rt_require_device)(u32 i), bool (*rt_release_device)(u32 i));
bool rt_device_write(u32 device_id,char * buf);
bool rt_device_write_byte(u32 device_id,char * buf, u32 i);

bool rt_device_read(u32 device_id, char * buf);

bool rt_require_device(u32 device_id, u32 num);
bool rt_release_device(u32 device_id, u32 num);
bool rt_claim_device(u32* require);

bool rt_device_write_by_num(u32 device_id, u32 num, char *);
bool rt_device_read_by_num(u32 device_id, u32 num, char *);

bool rt_task_exit();

#define LED_ID 0
#define SWITCH_ID 2
#define SEG_ID 1
#define BUTTON 3
#define VGA 4
#endif
