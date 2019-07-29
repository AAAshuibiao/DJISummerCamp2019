#include "referee.h"
#include "CRC8_CRC16.h"
#include "bsp_remote_control.h"
#include "cmsis_os.h"
#include "fifo.h"
#include "protocol.h"
#include "stdio.h"
#include "string.h"

//����ʽ��������
extern communicate_class_input_data_t communicate_input_data;
//����ʽ������
extern communicate_class_output_data_t communicate_output_data;

extern chassis_ctrl_info_t chassis_ctrl_input_data;

// USB����FIFO��ʼ��
void usb_fifo_init(void);

// USB FIFO���ƽṹ��
fifo_s_t usb_fifo;
// USB FIFO���λ�����
uint8_t usb_fifo_buf[512];
// RMЭ�������ƽṹ��
unpack_data_t referee_unpack_obj;
// RMЭ�鷴���л�����
void referee_unpack_fifo_data(void);

// RMЭ�����л�����
void referee_send_data(uint16_t cmd_id, void *buf, uint16_t len);

uint16_t communicate_result_pack(uint8_t *buf,
                                 communicate_class_output_data_t output);

extern void communicate_class_solve(void);
extern void chassis_command_solve();

frame_header_struct_t referee_receive_header;

// RMЭ�����������ϵͳ�Զ�����
void referee_task(void const *argument) {
  usb_fifo_init();
  while (1) {
    referee_unpack_fifo_data();
    osDelay(10);
  }
}

// USB FIFO��ʼ������
void usb_fifo_init(void) { fifo_s_init(&usb_fifo, usb_fifo_buf, 512); }

// USB�����ж�
void usb_receiver(uint8_t *buf, uint32_t len) {
  fifo_s_puts(&usb_fifo, (char *)buf, len);
}

uint16_t referee_data_solve(uint8_t *frame) {
  uint8_t index = 0;
  uint16_t cmd_id = 0;

  memcpy(&referee_receive_header, frame, sizeof(frame_header_struct_t));
  index += sizeof(frame_header_struct_t);

  memcpy(&cmd_id, frame + index, sizeof(uint16_t));
  index += sizeof(uint16_t);
  switch (cmd_id) {
  //���ռ���ʽ�������Ӧ��Ϣ��
  case CLASS_COM_CMD_ID: {
    memcpy(&communicate_input_data, frame + index,
           sizeof(communicate_class_input_data_t));
    communicate_class_solve();
    break;
  }

  case CHASSIS_CTRL_CMD_ID: {
    memcpy(&chassis_ctrl_input_data, frame + index,
           sizeof(chassis_ctrl_info_t));
    chassis_command_solve();
    break;
  }

  default: {
    break;
  }
  }

  index += referee_receive_header.data_length + 2;
  return index;
}
// RMЭ�鷴���л�
void referee_unpack_fifo_data(void) {
  uint8_t byte = 0;
  uint8_t sof = HEADER_SOF;
  unpack_data_t *p_obj = &referee_unpack_obj;

  while (fifo_s_used(&usb_fifo)) {
    byte = fifo_s_get(&usb_fifo);
    switch (p_obj->unpack_step) {
    //����֡ͷ
    case STEP_HEADER_SOF: {
      if (byte == sof) {
        p_obj->unpack_step = STEP_LENGTH_LOW;
        p_obj->protocol_packet[p_obj->index++] = byte;
      } else {
        p_obj->index = 0;
      }
    } break;

    //��ȡ���ݳ��ȵ��ֽ�
    case STEP_LENGTH_LOW: {
      p_obj->data_len = byte;
      p_obj->protocol_packet[p_obj->index++] = byte;
      p_obj->unpack_step = STEP_LENGTH_HIGH;
    } break;

    //��ȡ���ݳ��ȸ��ֽ�
    case STEP_LENGTH_HIGH: {
      p_obj->data_len |= (byte << 8);
      p_obj->protocol_packet[p_obj->index++] = byte;

      if (p_obj->data_len <
          (REF_PROTOCOL_FRAME_MAX_SIZE - REF_HEADER_CRC_CMDID_LEN)) {
        p_obj->unpack_step = STEP_FRAME_SEQ;
      } else {
        p_obj->unpack_step = STEP_HEADER_SOF;
        p_obj->index = 0;
      }
    } break;

    //��¼Э������к�
    case STEP_FRAME_SEQ: {
      p_obj->protocol_packet[p_obj->index++] = byte;
      p_obj->unpack_step = STEP_HEADER_CRC8;
    } break;

    //У��֡ͷCRC8
    case STEP_HEADER_CRC8: {
      p_obj->protocol_packet[p_obj->index++] = byte;

      if (p_obj->index == REF_PROTOCOL_HEADER_SIZE) {
        if (Verify_CRC8_Check_Sum(p_obj->protocol_packet,
                                  REF_PROTOCOL_HEADER_SIZE)) {
          p_obj->unpack_step = STEP_DATA_CRC16;
        } else {
          p_obj->unpack_step = STEP_HEADER_SOF;
          p_obj->index = 0;
        }
      }
    } break;

    //У����֡CRC16
    case STEP_DATA_CRC16: {
      if (p_obj->index < (REF_HEADER_CRC_CMDID_LEN + p_obj->data_len)) {
        p_obj->protocol_packet[p_obj->index++] = byte;
      }
      if (p_obj->index >= (REF_HEADER_CRC_CMDID_LEN + p_obj->data_len)) {
        p_obj->unpack_step = STEP_HEADER_SOF;
        p_obj->index = 0;

        if (Verify_CRC16_Check_Sum(p_obj->protocol_packet,
                                   REF_HEADER_CRC_CMDID_LEN +
                                       p_obj->data_len)) {
          //�ɹ��⵽һ����ȷ����Ϣ��
          referee_data_solve(p_obj->protocol_packet);
        }
      }
    } break;

    //���ʧ������Ѱ��֡ͷ
    default: {
      p_obj->unpack_step = STEP_HEADER_SOF;
      p_obj->index = 0;
    } break;
    }
  }
}
