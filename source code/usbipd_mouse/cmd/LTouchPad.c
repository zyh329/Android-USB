#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/usbdevice_fs.h>
#include <memory.h>
#include <malloc.h>

#define PORT 3700
#define MAX_RECV 10
#define MAX_SEND 1024
#define CMD_MAX 7
#define CMD_MAX_RECV 5
#define SIZE sizeof(struct sockaddr_in)
#define SIZE_USBDEVFS sizeof(struct usbdevfs_urb)
#define SIZE_ISOPACKETDESC sizeof(struct usbdevfs_iso_packet_desc)
#define TRUE 1
#define FALSE 0

enum CONTROL_ENUM {
	CONTROL_NUM, CLICK_L, CLICK_R,
	MOVE_L, MOVE_R, MOVE_U, MOVE_D,
	SCROLL_U, SCROLL_D, SCROLL_C
};

struct mouse_control {
	int data[10];
};

int set_urb(struct usbdevfs_urb *urb, struct *mouse_control);
int send_message(int sockfd, struct usbdevfs_urb urb);
int recv_cmd(int sockfd, char* cmd, int cmd_size);

int main(int argc, char* argv[])
{
	struct usbdevfs_urb urb;
	struct mouse_control m_control;
    struct sockaddr_in sockin;
	unsigned char buffer[4];
    int sockfd = -1;
	int con, cmd_num;
	char cmd[MAX_RECV];
	char c;

    if( (sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == -1) {
		fprintf(stderr, "socket() fail\n");
		return -1;
	}
	bzero((char*)&sockin,sizeof(sockin));
	sockin.sin_family = AF_INET;
	sockin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockin.sin_port = htons(PORT);

	con = connect(sockfd,(struct sockaddr*)&sockin, SIZE);
	if(con==-1) {
		fprintf(stderr, "connect() fail\n");
		return -1;
	}
	printf("connected : sockfd - %d\n", sockfd); 
	do {
		if(recv_cmd(sockfd, cmd, MAX_RECV)==-1){
			fprintf(stderr, "recv_cmd() fail\n");
			return -1;
		}
		if(sscanf(cmd, "%d", &cmd_num) != 1) {
			fprintf(stderr, "cmd_num error\n");
			return -1;
		}
		if(cmd_num<=CMD_MAX_RECV) {
			m_control[CONTROL_NUM] = cmd_num;
			set_urb(&urb, &m_control);
			send_message(sockfd, urb);
		}
		else
			printf("recv %d\n",cmd_num);
	}while(cmd_num<CMD_MAX);

	printf("enter key\n");
	do {
		c = getchar();
		fflush(stdin);

		// 버튼 다운, 업 따로 키설정
		// 방향설정일 때는 한번 더 보냄
		switch(c) {
		case '0' :
			m_control[CLICK_R] = !m_control[CLICK_R];
			break;
		case '1' :
			buffer[1] = 0xff;
			buffer[2] = 0x01;			
			break;
		case '2' :
			buffer[1] = 0x00;
			buffer[2] = 0x01;
			break;
		case '3' :
			buffer[1] = 0x01;
			buffer[2] = 0x01;
			break;
		case '4' :
			buffer[1] = 0xff;
			buffer[2] = 0x00;
			break;
		case '5' :
			m_control[CLICK_L] = !m_control[CLICK_L];
			break;
		case '6' :
			buffer[1] = 0x01;
			buffer[2] = 0x00;
			break;
		case '7' :
			buffer[1] = 0xff;
			buffer[2] = 0xff;
			break;
		case '8' :
			buffer[1] = 0x00;
			buffer[2] = 0xff;
			break;
		case '9' :
			buffer[1] = 0x01;
			buffer[2] = 0xff;
			break;
		case '/' :
			m_control[SCROLL_U] = TRUE;
			break;
		case '*' :
			m_control[SCROLL_C] = !m_control[SCROLL_C];
			break;
		case '-' :
			m_control[SCROLL_D] = TRUE;
			break;
		default :
			continue;
		}

/*
		switch(c) {
		case '0' :
			m_control[CLICK_R] = !m_control[CLICK_R];
			break;
		case '1' :
			m_control[MOVE_L] = TRUE;
			m_control[MOVE_D] = TRUE;			
			break;
		case '2' :
			m_control[MOVE_D] = TRUE;
			break;
		case '3' :
			m_control[MOVE_R] = TRUE;
			m_control[MOVE_D] = TRUE;
			break;
		case '4' :
			m_control[MOVE_L] = TRUE;
			break;
		case '5' :
			m_control[CLICK_L] = !m_control[CLICK_L];
			break;
		case '6' :
			m_control[MOVE_R] = TRUE;
			break;
		case '7' :
			m_control[MOVE_L] = TRUE;
			m_control[MOVE_U] = TRUE;
			break;
		case '8' :
			m_control[MOVE_U] = TRUE;
			break;
		case '9' :
			m_control[MOVE_R] = TRUE;
			m_control[MOVE_U] = TRUE;
			break;
		case '/' :
			m_control[SCROLL_U] = TRUE;
			break;
		case '*' :
			m_control[SCROLL_C] = !m_control[SCROLL_C];
			break;
		case '-' :
			m_control[SCROLL_D] = TRUE;
			break;
		default :
			continue;
		}
*/
		set_urb(&urb, &m_control);
		send_message(sockfd, urb);
	}while(1);
}

int recv_cmd(int sockfd, char* cmd, int cmd_size) 
{
	int ret, i=0;
	char cmd_tmp;

	//do {
		ret = recv(sockfd, &cmd_tmp, 1, 0);
		if(ret<=0) {
			fprintf(stderr, "recv() error\n");
			return -1;
		}
		cmd[i] = cmd_tmp;
		i++;
	//} while(i<cmd_size && cmd_tmp!='\0');
	return ret;
}

int send_message(int sockfd, struct usbdevfs_urb urb) {
	int ret, total = 0;
	ret = send(sockfd, &urb, SIZE_USBDEVFS, 0);
	if(ret<=0) {
		fprintf(stderr, "send(urb) error\n");
		return -1;
	}
	//printf("send(urb) : %d\n", ret);
	total += ret;
	ret = send(sockfd, buffer, urb.buffer_length, 0);
	if(ret<=0) {
		fprintf(stderr, "send(urb.buffer) error\n");
		return -1;
	}
	//printf("send(urb.buffer) : %d\n", ret);
	total += ret;
	ret = send(sockfd, urb.iso_frame_desc,
		SIZE_ISOPACKETDESC, 0);
	if(ret<=0) {
		fprintf(stderr, "send(urb.iso_frame_desc[0]) error\n");
		return -1;
	}
	//printf("send(urb.iso_frame_desc[0]) : %d\n", ret);
	total += ret;
	printf("send total : %d\n", total);
	return total;
}

int set_urb(struct usbdevfs_urb *urb, struct *mouse_control m_control)
{
	unsigned char buffer[MAX_SEND];
	static int seqnum = 0;

	if(m_control->data[CONTROL_NUM]>0) {

	switch(m_control->data[CONTROL_NUM]) {
	case 1 : 
		urb->type=2;
		urb->endpoint=0x80;
		urb->status=0;
		urb->flags=0x00000000;
        buffer[0]=0x80;
        buffer[1]=0x06;
        buffer[2]=0x00;
        buffer[3]=0x01;
        buffer[4]=0x00;
        buffer[5]=0x00;
        buffer[6]=0x12;
        buffer[7]=0x00;
        buffer[8]=0x12;
        buffer[9]=0x01;
        buffer[10]=0x00;
        buffer[11]=0x02;
        buffer[12]=0x00;
        buffer[13]=0x00;
        buffer[14]=0x00;
        buffer[15]=0x08;
        buffer[16]=0x61;
        buffer[17]=0x04;
        buffer[18]=0x16;
        buffer[19]=0x4d;
        buffer[20]=0x00;
        buffer[21]=0x02;
        buffer[22]=0x00;
        buffer[23]=0x02;
        buffer[24]=0x00;
        buffer[25]=0x01;
		urb->buffer_length=26;
		urb->actual_length=18;
		urb->start_frame=0;
		urb->number_of_packets=0;
		urb->error_count=0;
		urb->signr=0;
		urb->usercontext = 0x00;
		urb->iso_frame_desc[0].length = 0x01000680;
		urb->iso_frame_desc[0].actual_length = 0x00120000;
		urb->iso_frame_desc[0].status = 0x02000112;
		break;
	case 2 :
		urb->type=2;
		urb->endpoint=0x80;
		urb->status=0;
		urb->flags=0x00000000;
     	buffer[0]=0x80;
        buffer[1]=0x06;
        buffer[2]=0x00;
        buffer[3]=0x02;
        buffer[4]=0x00;
        buffer[5]=0x00;
        buffer[6]=0x09;
        buffer[7]=0x00;
        buffer[8]=0x09;
        buffer[9]=0x02;
        buffer[10]=0x22;
        buffer[11]=0x00;
        buffer[12]=0x01;
        buffer[13]=0x01;
        buffer[14]=0x00;
        buffer[15]=0xa0;
        buffer[16]=0x32;
		urb->buffer_length=17;
		urb->actual_length=9;
		urb->start_frame=0;
		urb->number_of_packets=0;
		urb->error_count=0;
		urb->signr=0;
		urb->usercontext = 0x00;
		urb->iso_frame_desc[0].length = 0x02000680;
		urb->iso_frame_desc[0].actual_length = 0x00090000;
		urb->iso_frame_desc[0].status = 0x00220209;
		break;
	case 3 : 
		urb->type=2;
		urb->endpoint=0x80;
		urb->status=0;
		urb->flags=0x00000000;
		buffer[0]=0x80;
        buffer[1]=0x06;
        buffer[2]=0x00;
        buffer[3]=0x02;
        buffer[4]=0x00;
        buffer[5]=0x00;
        buffer[6]=0x22;
        buffer[7]=0x00;
        buffer[8]=0x09;
        buffer[9]=0x02;
        buffer[10]=0x22;
        buffer[11]=0x00;
        buffer[12]=0x01;
        buffer[13]=0x01;
        buffer[14]=0x00;
        buffer[15]=0xa0;
        buffer[16]=0x32;
        buffer[17]=0x09;
        buffer[18]=0x04;
        buffer[19]=0x00;
        buffer[20]=0x00;
        buffer[21]=0x01;
        buffer[22]=0x03;
        buffer[23]=0x01;
        buffer[24]=0x02;
        buffer[25]=0x00;
        buffer[26]=0x09;
        buffer[27]=0x21;
        buffer[28]=0x11;
        buffer[29]=0x01;
        buffer[30]=0x00;
        buffer[31]=0x01;
        buffer[32]=0x22;
        buffer[33]=0x34;
        buffer[34]=0x00;
        buffer[35]=0x07;
        buffer[36]=0x05;
        buffer[37]=0x81;
        buffer[38]=0x03;
        buffer[39]=0x04;
        buffer[40]=0x00;
        buffer[41]=0x0a;
		urb->buffer_length=42;
		urb->actual_length=34;
		urb->start_frame=0;
		urb->number_of_packets=0;
		urb->error_count=0;
		urb->signr=0;
		urb->usercontext = 0x00;
		urb->iso_frame_desc[0].length = 0x02000680;
		urb->iso_frame_desc[0].actual_length = 0x00220000;
		urb->iso_frame_desc[0].status = 0x00220209;
		break;
	case 4 : 
		urb->type=2;
		urb->endpoint=0x00;
		urb->status=0;
		urb->flags=0x00000000;
		buffer[0]=0x21;
        buffer[1]=0x0a;
        buffer[2]=0x00;
        buffer[3]=0x00;
        buffer[4]=0x00;
        buffer[5]=0x00;
        buffer[6]=0x00;
        buffer[7]=0x00;
		urb->buffer_length=8;
		urb->actual_length=0;
		urb->start_frame=0;
		urb->number_of_packets=0;
		urb->error_count=0;
		urb->signr=0;
		urb->usercontext = 0x00;
		urb->iso_frame_desc[0].length = 0x00000a21;
		urb->iso_frame_desc[0].actual_length = 0x00000000;
		urb->iso_frame_desc[0].status = 0x0001d881;
		break;
	case 5 : 
		urb->type=2;
		urb->endpoint=0x80;
		urb->status=0;
		urb->flags=0x00000000;
		buffer[0]=0x81;
        buffer[1]=0x06;
        buffer[2]=0x00;
        buffer[3]=0x22;
        buffer[4]=0x00;
        buffer[5]=0x00;
        buffer[6]=0x74;
        buffer[7]=0x00;
        buffer[8]=0x05;
        buffer[9]=0x01;
        buffer[10]=0x09;
        buffer[11]=0x02;
        buffer[12]=0xa1;
        buffer[13]=0x01;
        buffer[14]=0x09;
        buffer[15]=0x01;
        buffer[16]=0xa1;
        buffer[17]=0x00;
        buffer[18]=0x05;
        buffer[19]=0x09;
        buffer[20]=0x19;
        buffer[21]=0x01;
        buffer[22]=0x29;
        buffer[23]=0x03;
        buffer[24]=0x15;
        buffer[25]=0x00;
        buffer[26]=0x25;
        buffer[27]=0x01;
        buffer[28]=0x75;
        buffer[29]=0x01;
        buffer[30]=0x95;
        buffer[31]=0x03;
        buffer[32]=0x81;
        buffer[33]=0x02;
        buffer[34]=0x75;
        buffer[35]=0x05;
        buffer[36]=0x95;
        buffer[37]=0x01;
        buffer[38]=0x81;
        buffer[39]=0x01;
        buffer[40]=0x05;
        buffer[41]=0x01;
        buffer[42]=0x09;
        buffer[43]=0x30;
        buffer[44]=0x09;
        buffer[45]=0x31;
        buffer[46]=0x09;
        buffer[47]=0x38;
        buffer[48]=0x15;
        buffer[49]=0x81;
        buffer[50]=0x25;
        buffer[51]=0x7f;
        buffer[52]=0x75;
        buffer[53]=0x08;
        buffer[54]=0x95;
        buffer[55]=0x03;
        buffer[56]=0x81;
        buffer[57]=0x06;
        buffer[58]=0xc0;
        buffer[59]=0xc0;
        buffer[60]=0x00;
        buffer[61]=0x00;
        buffer[62]=0x00;
        buffer[63]=0x00;
        buffer[64]=0x70;
        buffer[65]=0x68;
        buffer[66]=0xa5;
        buffer[67]=0x01;
        buffer[68]=0x00;
        buffer[69]=0x00;
        buffer[70]=0x00;
        buffer[71]=0x00;
        buffer[72]=0x03;
        buffer[73]=0x00;
        buffer[74]=0x00;
        buffer[75]=0x00;
        buffer[76]=0x00;
        buffer[77]=0x00;
        buffer[78]=0x00;
        buffer[79]=0x00;
        buffer[80]=0x2a;
        buffer[81]=0x00;
        buffer[82]=0x00;
        buffer[83]=0x00;
        buffer[84]=0x22;
        buffer[85]=0x00;
        buffer[86]=0x00;
        buffer[87]=0x00;
        buffer[88]=0x02;
        buffer[89]=0x80;
        buffer[90]=0x00;
        buffer[91]=0x00;
        buffer[92]=0x00;
        buffer[93]=0x00;
        buffer[94]=0x00;
        buffer[95]=0x00;
        buffer[96]=0x00;
        buffer[97]=0x00;
        buffer[98]=0x00;
        buffer[99]=0x00;
        buffer[100]=0x00;
        buffer[101]=0x00;
        buffer[102]=0x00;
        buffer[103]=0x00;
        buffer[104]=0x70;
        buffer[105]=0x68;
        buffer[106]=0xa5;
        buffer[107]=0x01;
        buffer[108]=0x00;
        buffer[109]=0x00;
        buffer[110]=0x00;
        buffer[111]=0x00;
        buffer[112]=0x2a;
        buffer[113]=0x00;
        buffer[114]=0x00;
        buffer[115]=0x00;
        buffer[116]=0x22;
        buffer[117]=0x00;
        buffer[118]=0x00;
        buffer[119]=0x00;
        buffer[120]=0x00;
        buffer[121]=0x00;
        buffer[122]=0x00;
        buffer[123]=0x00;
		urb->buffer_length=124;
		urb->actual_length=52;
		urb->start_frame=0;
		urb->number_of_packets=0;
		urb->error_count=0;
		urb->signr=0;
		urb->usercontext = 0x00;
		urb->iso_frame_desc[0].length = 0x22000681;
		urb->iso_frame_desc[0].actual_length = 0x00740000;
		urb->iso_frame_desc[0].status = 0x02090105;
		break;
	default :
		fprintf(stderr, "set_urb cmd_num error\n");
		return -1;
	} // switch end
		if(urb->buffer_length>MAX_SEND) {
			fprintf(stderr, "buffer MAX size over\n");
			return -1;
		}
		if(urb->buffer)
			free(urb->buffer);
		urb->buffer = (unsigned cahr*)malloc(urb->buffer_length);
		memcpy(urb->buffer, buffer, urb->buffer_length);
	}

	else {
		urb->type=1;
		urb->endpoint=0x81;
		urb->status=0;
		urb->flags=0x00000000;
		urb->buffer_length=4;
		urb->actual_length=4;
		urb->start_frame=0;
		urb->number_of_packets=0;
		urb->error_count=0;
		urb->signr=0;
		urb->usercontext = 0x00;

		if(m_control->data[CLICK_L]) {
			buffer[0] = 0x01;
		}
		if(m_control->data[CLICK_R]) {
			buffer[0] = 0x02;
		}
		if(m_control->data[SCROLL_C]) {
			buffer[0] = 0x04;
		}
		if(m_control->data[MOVE_L]) {
			buffer[1] = 0xff;
			m_control->data[MOVE_L] = FALSE;
		}
		else if(m_control->data[MOVE_R]) {
			buffer[1] = 0x01;
			m_control->data[MOVE_R] = FALSE;
		}
		if(m_control->data[MOVE_U]) {
			buffer[2] = 0xff;
			m_control->data[MOVE_U] = FALSE;
		}
		else if(m_control->data[MOVE_D]) {
			buffer[2] = 0x01;
			m_control->data[MOVE_D] = FALSE;
		}
		if(m_control->data[SCROLL_D]) {
			buffer[3] = 0x01;
			m_control->data[SCROLL_D] = FALSE;
		}
		else if(m_control->data[MOVE_D
		

/*
	CONTROL_NUM, CLICK_L, CLICK_R,
	MOVE_L, MOVE_R, MOVE_U, MOVE_D,
	SCROLL_U, SCROLL_D, SCROLL_C
*/

	case MOUSE_MOVE2 : 
        buffer[0]=0x00;
        buffer[1]=0x06;
        buffer[2]=0xff;
        buffer[3]=0x00;
		urb.iso_frame_desc[0].actual_length = 0x00000000;
		urb.iso_frame_desc[0].status = 0x00000061;
		urb.iso_frame_desc[0].actual_length = 0x00740000;
		urb.iso_frame_desc[0].status = 0x00000021;

		buffer[0]=0x00;
        buffer[1]=0x01;
        buffer[2]=0x01;
        buffer[3]=0x00;
		urb.iso_frame_desc[0].length = 0x00010100;

	return 0;
}

