//v0.114514
//����ΰ��ķ���D��ʦ�İ汾ħ�Ķ���

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define Param_fname "dict.dic"/*�����ĵ�*/
#define Max_buffer 1000
//���鳤�����ֵ
#define Max_single_buf 20
//���ַ��������������鳤�����ֵ
#define Num_param 3
//����������
#define Num_cmd 5
//ָ��������
enum def_strategy {
	FCFS/*�����ȷ���*/, SSTF/*���ʱ��*/, SCAN /*˳�����*/
	//��name_strategyһ�£����ų���0~2
};

enum def_cmd {
	CLK, END, TGT/*target*/, CLKW/*clockwise*/, CCLKW /*counterclockwise*/
	//cmdָ�����name_cmdһ�£����ų���0~5
};

const char *name_param[Num_param] = {(char *)"TOTAL_STATION", (char *)"STRATEGY", (char *)"DISTANCE"};

const char *name_strategy[Num_param] = {(char *)"FCFS", (char *)"SSTF", (char *)"SCAN"};

const char *name_cmd[Num_cmd] = {(char *)"clock", (char *)"end", (char *)"target", (char *)"clockwise", (char *)"counterclockwise"};

enum def_bus {
	Start, End, Going, Stop
	//������״̬��
};

struct Sta_req {
	int req_num;
	int time_send;
	int time_finish;
	int time_wait;
	int req_type;
	int req_place;
	//ԭʼ������нṹ
};

struct Sta_stat {
	int stat_num;
	int stat_clkw;
	int stat_cclkw;
	int stat_tgt;
	int judgement;//���judgement,1����Ŀ��վ��0����˳��վ��-1��������վ
	int stat_sec;//ʱ���1�����µ����ʱ�䣩
	int order_sec;//ʱ���2����ȷ��ΪĿ��վ��ʱ�䣩
	int stat_distance;//����Ŀ��վ���������ڱ�ȷ��ΪĿ��վ����1sʱ�복�ľ���
	int stat_clkw_order;
	int stat_cclkw_order;
	int stat_tgt_order;

	//վ��״̬����
};

struct Sta_bus {
	int bus_pos;
	int bus_dir;
	int bus_state;
	//bus״̬
};

//�ַ���ƥ��
int str_match(const char *name_str[], int num, char buf[]) {
	//����str_name����ȷƥ��buf���±�
	for (int i = 0; i < num; i++)
		if (strcmp(buf, name_str[i]) == 0)
			return i;
	//�����ַ��Ƚ�Ҳ��
	return -1;
}

//���ò���
void set_param(int *total_station, int *strategy, int *distance, int *total_distance) {
	FILE *fp;
	if ((fp = fopen(Param_fname, "r")) == NULL) {
		fprintf(stderr, "can't open the file \"%s\"\n", Param_fname);
		exit(1);
	}

	char buffer[Max_buffer];
	while (fgets(buffer, Max_buffer, fp) != NULL) {
		if (buffer[0] != '#') {
			int i = 0;
			char single_buf[Max_single_buf];
			while (buffer[i] != ' ') {
				single_buf[i] = buffer[i];
				i++;
			}
			single_buf[i] = 0;
			//���뵥��

			int num = str_match(name_param, Num_param, single_buf);
			//�ַ���ƥ��
			i += 3;
			//�����Ⱥţ��ٶ��ȺŸ�ʽ�Ϸ���
			//�ٶ���Ч�����ݱ�֤�Ϸ�
			//TOTAL_STATION��2~10
			//DISTANCE��1~5
			switch (num) {
				case 0:
					*total_station = atoi(&buffer[i]);
					break;
				//atoi���� ���Ⱥ�֮����ַ���ת��Ϊ��������
				case 1: {
					int j = 0;
					while (buffer[i] != '\n')
						//fgets����س����з�
					{
						single_buf[j] = buffer[i];
						i++;
						j++;
					}
					single_buf[j] = 0;
					//÷�����ȷ��뵥��
					*strategy = str_match(name_strategy, Num_param, single_buf);
					//÷�������ַ���ƥ��
					break;
				}
				case 2:
					*distance = atoi(&buffer[i]);
					break;
				default:
					printf("invalid param name\n");
					break;
			}
		}
	}
	*total_distance = *total_station **distance;

	if (fclose(fp) != 0)
		fprintf(stderr, "Error in closing files\n");
}

//��ָ��
int get_cmd() {
	char single_buf[Max_single_buf];
	scanf("%s", single_buf);
	return str_match(name_cmd, Num_cmd, single_buf);
}

//���г�ʼ��
void que_init(Sta_req original[], Sta_stat station[], int total_station) {
	//��������±�1��ʼ��
	for (int i = 0; i < Max_buffer; i++) {
		original[i].req_num = i;
		original[i].time_send = -1;
		original[i].time_finish = -1;
		original[i].time_wait = -1;
		original[i].req_type = -1;
		original[i].req_place = -1;
	}
	for (int i = 0; i < total_station + 1; i++) {
		station[i].stat_num = i;
		station[i].stat_clkw = 0;
		station[i].stat_cclkw = 0;
		station[i].stat_tgt = 0;
		station[i].judgement = -1;//��ʼ��
		station[i].stat_sec = -1;
		station[i].order_sec = 1000000;
		station[i].stat_distance = 1000000;
		station[i].stat_clkw_order = 1000000;
		station[i].stat_cclkw_order = 1000000;
		station[i].stat_tgt_order = 1000000;
	}
}

//������״̬��
void bus_run(Sta_bus *bus, int total_distance) {
	switch ((*bus).bus_state) {
		case Start:
			break;
		case End:
			break;
		case Going:
			if ((*bus).bus_dir == CLKW)
				((*bus).bus_pos)++;
			else if ((*bus).bus_dir == CCLKW)
				((*bus).bus_pos)--;
			break;
		case Stop:
			break;
		default:
			printf("invalid bus state\n");
			break;
	}
	if ((*bus).bus_pos < 0)
		(*bus).bus_pos = total_distance - 1;
	else if ((*bus).bus_pos > total_distance - 1)
		(*bus).bus_pos = 0;

}

//��ȡ�������
void queue_in(int cmd, int *cnt_tgt, int sec, Sta_req original[], Sta_stat station[], int order) {
	int tgt;
	scanf("%d", &tgt);
	//�����豣ָ֤��������һ����
	//�����豣֤����Խ��
	int isquit = 1;
	switch (cmd) {
		case TGT:
			if (station[tgt].stat_tgt == 0) {
				station[tgt].stat_tgt = 1;
				station[tgt].stat_sec = sec;
				if (order < station[tgt].stat_tgt_order) {
					station[tgt].stat_tgt_order = order;
				}
				isquit = 0;
			}
			break;
		case CLKW:
			if (station[tgt].stat_clkw == 0) {
				station[tgt].stat_clkw = 1;
				station[tgt].stat_sec = sec;
				if (order < station[tgt].stat_clkw_order) {
					station[tgt].stat_clkw_order = order;
				}
				isquit = 0;
			}
			break;
		case CCLKW:
			if (station[tgt].stat_cclkw == 0) {
				station[tgt].stat_cclkw = 1;
				station[tgt].stat_sec = sec;
				if (order < station[tgt].stat_cclkw_order) {
					station[tgt].stat_cclkw_order = order;
				}
				isquit = 0;
			}
			break;
		default:
			printf("invalid cmd name\n");
			break;
			if (isquit == 0) {
				*cnt_tgt++;
				original[*cnt_tgt].time_send = sec;
				original[*cnt_tgt].req_type = cmd;
				original[*cnt_tgt].req_place = tgt;
			}
	}
}

//���
void info_prin(int sec, Sta_bus bus, Sta_stat station[], int total_station) {
	printf("TIME:%d\n", sec);
	printf("BUS:\n");
	printf("position:%d\n", bus.bus_pos);
	printf("target: ");
	for (int i = 1; i < total_station + 1; i++)
		printf("%d", station[i].stat_tgt);
	printf("\n");
	printf("STATION:\n");
	printf("clockwise: ");
	for (int i = 1; i < total_station + 1; i++)
		printf("%d", station[i].stat_clkw);
	printf("\n");
	printf("counterclockwise: ");
	for (int i = 1; i < total_station + 1; i++)
		printf("%d", station[i].stat_cclkw);
	printf("\n");
}

//��ӵĺ���1������ȷ��Ŀ��վ��˳��վ
void dispatch(Sta_stat station[], int total_station, int total_distance, int distance, Sta_bus *bus, int sec) {
	int t = 0, a1 = 3, statsec = -1;
	for (int i = 1; i <= total_station; i++) {
		if (station[i].judgement == 1) {
			t = i;//Ѱ���Ƿ���Ŀ��վ
			statsec = station[i].stat_sec;
			break;
		}
	}

	if (t != 0 && statsec == sec) {
		for (int i = 1; i <= total_station; i++);
	}





	if (t != 0) {//��Ŀ��վʱ��ȷ��˳��վ
		int k = ((*bus).bus_pos) / distance + 1;
		if ((*bus).bus_dir == CLKW) {//˳ʱ��
			if (k < station[t].stat_num) {//���������վ���ڳ��������
				for (int i = k + 1; i < station[t].stat_num; i++) {
					if (station[i].stat_clkw == 1 || station[i].stat_tgt == 1)
						station[i].judgement = 0;//ȷ��Ϊ˳��վ
				}
			} else if (k > station[t].stat_num) {
				for (int i = 1; i <= total_station; i++) {
					if ((i < t || i > k) && (station[i].stat_clkw == 1 || station[i].stat_tgt == 1))
						station[i].judgement = 0;//ȷ��Ϊ˳��վ
				}

			}
		} else if ((*bus).bus_dir == CCLKW) {//��ʱ��
			if (k > station[t].stat_num) {//���������վ���ڳ��������
				for (int i = k + 1; i > station[t].stat_num; i--) {
					if (station[i].stat_cclkw == 1 || station[i].stat_tgt == 1)
						station[i].judgement = 0;//ȷ��Ϊ˳��վ
				}
			} else if (k < station[t].stat_num) {
				for (int i = 1; i <= total_station; i++) {
					if ((i <= k || i > t) && (station[i].stat_cclkw == 1 || station[i].stat_tgt == 1))
						station[i].judgement = 0;//ȷ��Ϊ˳��վ
				}
			}

		}
	}

	else if (t == 0) {//��Ŀ��վʱ��ȷ��Ŀ��վ
		int judg1[total_station + 3];//����judg����ֱ����ڴ洢˳������³��ĳ�վ�복λ�ò�ľ���ֵ
		int judg2[total_station + 3];
		int judg3[total_station + 3];
		for (int i = 1; i <= total_station; i++) {
			judg1[i] = 3000;
			judg2[i] = 3000;
			judg3[i] = 3000;
		}
		int stat1 = 0, stat2 = 0, stat3 = 0, min1 = 10000000, min2 = 10000000,
		    min3 = 10000000;//state�洢վ���,minѰ����������·ֱ����Сֵ
		for (int i = 1; i <= total_station; i++) {
			if (station[i].stat_clkw == 1) {//�жϵ�iվ�Ƿ���˳ʱ������
				if ((station[i].stat_num - 1) * distance >= (*bus).bus_pos) {
					judg1[i] = (station[i].stat_num - 1) * distance - (*bus).bus_pos;
				} else
					judg1[i] = total_distance - (*bus).bus_pos + (station[i].stat_num - 1) * distance;
			}
			if (station[i].stat_cclkw == 1) {//�жϵ�iվ�Ƿ�����ʱ������
				if ((station[i].stat_num - 1) * distance >= (*bus).bus_pos) {
					judg2[i] = (*bus).bus_pos + total_distance - (station[i].stat_num - 1) * distance;
				} else
					judg2[i] = (*bus).bus_pos - (station[i].stat_num - 1) * distance;
			}
			if (station[i].stat_tgt == 1) {//�жϵ�iվ�Ƿ����³�����
				if ((station[i].stat_num - 1) * distance >= (*bus).bus_pos) {//�³�����ֱ�Ѱ��˳ʱ�����������ʱ�����
					int s1 = (station[i].stat_num - 1) * distance - (*bus).bus_pos;
					int s2 = (*bus).bus_pos + total_distance - (station[i].stat_num - 1) * distance;
					if (s1 <= s2) {
						judg3[i] = s1;
						a1 = 0;
					}

					else {
						judg3[i] = s2;
						a1 = 1;
					}

				} else {
					int s3 = total_distance - (*bus).bus_pos + (station[i].stat_num - 1) * distance;
					int s4 = (*bus).bus_pos - (station[i].stat_num - 1) * distance;
					if (s3 <= s4) {
						judg3[i] = s3;
						a1 = 0;
					}

					else {
						judg3[i] = s4;
						a1 = 1;
					}

				}
			}
		}
		for (int i = 1; i <= total_station; i++) {//˳ʱ��Ѱ����С���뼰��վ̨
			if (min1 > judg1[i]) {
				min1 = judg1[i];
				stat1 = i;
			}
		}
		for (int i = 1; i <= total_station; i++) {//��ʱ��Ѱ����С���뼰��վ̨
			if (min2 > judg2[i]) {
				min2 = judg2[i];
				stat2 = i;
			}
		}
		for (int i = 1; i <= total_station; i++) {//�³�Ѱ����С���뼰��վ̨
			if (min3 > judg3[i]) {
				min3 = judg3[i];
				stat3 = i;
			}
		}


		if (min1 < min2 && min1 < min3) {//˳ʱ����С��ȷ������ʻ�����Ŀ��վ
			if ((station[stat1].stat_num - 1) * distance >= (*bus).bus_pos) {
				int q1 = (station[stat1].stat_num - 1) * distance - (*bus).bus_pos;
				int q2 = (*bus).bus_pos + total_distance - (station[stat1].stat_num - 1) * distance;
				if (q1 <= q2) {
					(*bus).bus_dir = CLKW;
				}

				else {
					(*bus).bus_dir = CCLKW;
				}
			}
			else {
				int q3 = (*bus).bus_pos - (station[stat1].stat_num - 1) * distance;
				int q4 = (station[stat1].stat_num - 1) * distance - (*bus).bus_pos + total_distance;
				if (q3 <= q4) {
					(*bus).bus_dir = CLKW;
				}

				else {
					(*bus).bus_dir = CCLKW;
				}
			}
			station[stat1].judgement = 1;
			station[stat1].order_sec = sec;
			station[stat1].stat_distance = min1;
		} else if (min2 < min1 && min2 < min3) {//��ʱ����С��ȷ������ʻ�����Ŀ��վ
			if ((station[stat2].stat_num - 1) * distance >= (*bus).bus_pos) {
				int q1 = (station[stat2].stat_num - 1) * distance - (*bus).bus_pos;
				int q2 = (*bus).bus_pos + total_distance - (station[stat2].stat_num - 1) * distance;
				if (q1 <= q2) {
					(*bus).bus_dir = CLKW;
				}

				else {
					(*bus).bus_dir = CCLKW;
				}
			}
			else {
				int q3 = (*bus).bus_pos - (station[stat2].stat_num - 1) * distance;
				int q4 = (station[stat2].stat_num - 1) * distance - (*bus).bus_pos + total_distance;
				if (q3 <= q4) {
					(*bus).bus_dir = CLKW;
				}

				else {
					(*bus).bus_dir = CCLKW;
				}
			}
			station[stat2].judgement = 1;
			station[stat2].order_sec = sec;
			station[stat2].stat_distance = min2;
		} else if (min3 < min1 && min3 < min2) {//�³���С��ȷ������ʻ�����Ŀ��վ
			if (a1 == 0)
				(*bus).bus_dir = CLKW;
			else if (a1 == 1)
				(*bus).bus_dir = CCLKW;
			station[stat3].judgement = 1;
			station[stat2].order_sec = sec;
			station[stat3].stat_distance = min3;
		} else if (min1 == min2 && min1 == min3) {
			if (stat1 == stat2) {
				if (station[stat1].stat_clkw_order < station[stat3].stat_tgt_order) {
					(*bus).bus_dir = CLKW;
					station[stat1].judgement = 1;
					station[stat1].order_sec = sec;
					station[stat1].stat_distance = min1;

				} else if (station[stat1].stat_clkw_order > station[stat3].stat_tgt_order) {
					if (a1 == 0)
						(*bus).bus_dir = CLKW;
					else if (a1 == 1)
						(*bus).bus_dir = CCLKW;
					station[stat3].judgement = 1;
					station[stat2].order_sec = sec;
					station[stat3].stat_distance = min3;
				}
			} else if (stat1 == stat3) {
				if (station[stat1].stat_clkw_order < station[stat2].stat_cclkw_order) {
					(*bus).bus_dir = CLKW;
					station[stat1].judgement = 1;
					station[stat1].order_sec = sec;
					station[stat1].stat_distance = min1;
				} else if (station[stat1].stat_clkw_order > station[stat2].stat_cclkw_order) {
					(*bus).bus_dir = CCLKW;
					station[stat2].judgement = 1;
					station[stat2].order_sec = sec;
					station[stat2].stat_distance = min2;
				}
			}
		}




		else if (min1 == min2) {
			if (station[stat1].stat_clkw_order < station[stat2].stat_cclkw_order) {
				(*bus).bus_dir = CLKW;
				station[stat1].judgement = 1;
				station[stat1].order_sec = sec;
				station[stat1].stat_distance = min1;
			} else if (station[stat1].stat_clkw_order > station[stat2].stat_cclkw_order) {
				(*bus).bus_dir = CCLKW;
				station[stat2].judgement = 1;
				station[stat2].order_sec = sec;
				station[stat2].stat_distance = min2;
			}
		} else if (min1 == min3) {
			if (station[stat1].stat_clkw_order < station[stat3].stat_tgt_order) {
				(*bus).bus_dir = CLKW;
				station[stat1].judgement = 1;
				station[stat1].order_sec = sec;
				station[stat1].stat_distance = min1;

			} else if (station[stat1].stat_clkw_order > station[stat3].stat_tgt_order) {
				if (a1 == 0)
					(*bus).bus_dir = CLKW;
				else if (a1 == 1)
					(*bus).bus_dir = CCLKW;
				station[stat3].judgement = 1;
				station[stat2].order_sec = sec;
				station[stat3].stat_distance = min3;
			}
		} else if (min2 == min3) {
			if (station[stat2].stat_cclkw_order < station[stat3].stat_tgt_order) {
				(*bus).bus_dir = CCLKW;
				station[stat2].judgement = 1;
				station[stat2].order_sec = sec;
				station[stat2].stat_distance = min2;
			} else if (station[stat2].stat_cclkw_order > station[stat3].stat_tgt_order) {
				if (a1 == 0)
					(*bus).bus_dir = CLKW;
				else if (a1 == 1)
					(*bus).bus_dir = CCLKW;
				station[stat3].judgement = 1;
				station[stat2].order_sec = sec;
				station[stat3].stat_distance = min3;
			}
		}
	}
}
//��ӵĺ���2��������ʻ��վ̨λ��ʱ��վ̨���в���
void station_operation(Sta_stat station[], int total_station, int total_distance, int distance,

                       int *stop_judg,//��ӵı���������ʵ���ó���վ̨ͣ1s���������н���ʵ��ԭ��
                       Sta_bus *bus, int sec) {
	int i = (*bus).bus_pos / distance + 1;//��ӵı�����i=վ̨��
	switch (station[i].judgement) {
		case -1://����վ,�������
			break;
		case 0://˳��վ��ͬ�����³���0
			station[i].stat_tgt = 0;
			station[i].stat_tgt_order = 1000000;
			if ((*bus).bus_dir == CLKW) {
				station[i].stat_clkw = 0;
				station[i].stat_clkw_order = 1000000;
			}

			else if ((*bus).bus_dir == CCLKW) {
				station[i].stat_cclkw = 0;
				station[i].stat_cclkw_order = 1000000;
			}

			station[i].judgement = -1;//��˳��վ��Ϊ����վ
			*stop_judg = 1;
			(*bus).bus_state = Stop;//������״̬��Going��ΪStop
			break;
		case 1:
			station[i].stat_tgt = 0;
			station[i].stat_clkw = 0;
			station[i].stat_cclkw = 0;
			station[i].stat_clkw_order = 1000000;
			station[i].stat_cclkw_order = 1000000;
			station[i].stat_tgt_order = 1000000;
			station[i].judgement = -1;//��Ŀ��վ��Ϊ����վ
			*stop_judg = 1;
			(*bus).bus_state = Stop;//������״̬��Going��ΪStop
			dispatch(station, total_station, total_distance, distance, bus, sec); //����Ѱ����һ��Ŀ��վ
			break;
	}

}



//������
int main(void) {
	int total_station = 5;
	int strategy = FCFS;
	int distance = 2;
	int total_distance = total_station * distance;
	//��ʱ:λ�ã�0 1 2 3 4 5 6 7 8 9 (0)ѭ��
	//     ��վ��1   2   3   4   5   (1)//λ��=��վ*2-2
	set_param(&total_station, &strategy, &distance, &total_distance); //���ò���
//	printf("%d %d %d %d\n",total_station,strategy,distance,total_distance);//���Բ�������

	Sta_req original[Max_buffer];//ԭʼ�������//����Ϊָ������Max_buffer��1000//���޸ģ���̬����
	Sta_stat station[total_station + 1]; //վ��״̬����//�Ա䳤����������
	que_init(original, station, total_station); //���г�ʼ��

	int sec = -1; //ʱ�����
	int cnt_tgt = 0; //�������
	Sta_bus bus = {0, CLKW, Start}; //bus��״̬

	int order = 0;

	int stop_judg = 0;
	//��ӵı���������ʵ���ó���վ̨ͣ1s��
	// ��ʼʱ��0��������ʻ��˳��վ��Ŀ��վʱ�����ú�������ַ�����Ϊ1������״̬��ΪStop
	//�����ܿ������"��stop_judg����0������վ�㣬������Ŀ��վ�򽫳���״̬��ΪGoing"�Ĺ���1����ʵ��ԭ��ͣ��1s��
	// ͬʱ��Ϊ���ó�����ͣ��1s���ڵڶ���ѭ���Ŀ�ʼλ�ã����빦��2����stop_judg����1������վ�㣬
	// ������Ŀ��վ��stop_judg��Ϊ0���������ٴ����е�����
	//ʱ����״̬�Ϳ��Ը�ΪGoing,��һ��clock�Ϳ��Լ������ˡ�

	int cmd = CLK; //����0��ʱ�����
	while (cmd != END) {
		if (cmd == CLK) {
			sec++;
			bus_run(&bus, total_distance); //������״̬��
			info_prin(sec, bus, station, total_station); //���
			if (stop_judg == 1) {//�����ᵽ�Ĺ���2
				for (int i = 1; i <= total_station; i++) {
					if (station[i].judgement == 1) {
						stop_judg = 0;
						break;
					}
				}
			}
			if ((bus.bus_pos) % distance == 0) {//վ��λ�ã����ú���������վ��
				station_operation(station, total_station, total_distance, distance, &stop_judg, &bus, sec);
			}
		} else if (cmd == TGT || cmd == CLKW || cmd == CCLKW) {
			order++;
			queue_in(cmd, &cnt_tgt, sec, original, station, order); //��ȡ�������
			//������:)
			dispatch(station, total_station, total_distance, distance, &bus, sec); //������ӵģ��ж�վ��ķ����Ƿ���Ҫ���
		} else
			printf("invalid cmd name\n");

		if (stop_judg == 0) {//�����ᵽ�Ĺ���1
			for (int i = 1; i <= total_station; i++) {
				if (station[i].judgement == 1) {
					bus.bus_state = Going;
					break;
				}
			}
		}
		cmd = get_cmd(); //��ָ��
	}
	printf("end\n");

//	system("pause");
	return 0;
}