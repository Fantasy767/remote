#include "app_transmit.h"
#include "SI24R1.h"
#include "app_data.h"
extern remote_data  re_data;
uint8_t  tx_data[TX_PLOAD_WIDTH]={0};
void app_transmit_data()
{
    SI24R1_TX_Mode();
    uint32_t  check_sum=0;
    uint8_t off_or_on;
    //发送数据
    //帧头校验，保证唯一性和可靠性
    tx_data[0]=FIRST_HEAD_CHECK;
    tx_data[1]=SECOND_HEAD_CHECK;
    tx_data[2]=THIRD_HEAD_CHECK;

    tx_data[3]=(re_data.thr>>8)&0xff;
    tx_data[4]=re_data.thr&0xff;

    tx_data[5]=(re_data.yaw>>8)&0xff;
    tx_data[6]=re_data.yaw&0xff;

    tx_data[7]=(re_data.pitch>>8)&0xff;
    tx_data[8]=re_data.pitch&0xff;

    tx_data[9]=(re_data.roll>>8)&0xff;
    tx_data[10]=re_data.roll&0xff;

    taskENTER_CRITICAL();
    tx_data[11]=re_data.off_or_on;//如果正好在此时任务被打断，？？？？what  happend
    off_or_on = tx_data[11];
    re_data.off_or_on=0;
    tx_data[12]=re_data.height;
    re_data.height=0;
    taskEXIT_CRITICAL();
    for(uint8_t i=0;i<13;++i)
    {
        check_sum+=tx_data[i];
    }
    tx_data[13]=(check_sum>>24)&0xff;
    tx_data[14]=(check_sum>>16)&0xff;
    tx_data[15]=(check_sum>>8)&0xff;
    tx_data[16]=(check_sum>>0)&0xff;

    uint8_t tx_result = SI24R1_TxPacket(tx_data);
    debug_printf("tx=%d off_or_on=%d\r\n", tx_result, off_or_on);

    SI24R1_RX_Mode();

}
