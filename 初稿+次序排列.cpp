//v0.114514
//基于伟大的伏培燚大师的版本魔改而成

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define Param_fname "dict.dic"/*测试文档*/
#define Max_buffer 1000
//数组长度最大值
#define Max_single_buf 20
//单字符串解析缓冲数组长度最大值
#define Num_param 3
//参数种类数
#define Num_cmd 5
//指令种类数
enum def_strategy {
	FCFS/*先来先服务*/, SSTF/*最短时间*/, SCAN /*顺便服务*/
	//与name_strategy一致，符号常量0~2
};

enum def_cmd {
	CLK, END, TGT/*target*/, CLKW/*clockwise*/, CCLKW /*counterclockwise*/
	//cmd指令集，与name_cmd一致，符号常量0~5
};

const char *name_param[Num_param] = {(char *)"TOTAL_STATION", (char *)"STRATEGY", (char *)"DISTANCE"};

const char *name_strategy[Num_param] = {(char *)"FCFS", (char *)"SSTF", (char *)"SCAN"};

const char *name_cmd[Num_cmd] = {(char *)"clock", (char *)"end", (char *)"target", (char *)"clockwise", (char *)"counterclockwise"};

enum def_bus {
	Start, End, Going, Stop
	//公交车状态集
};

struct Sta_req {
	int req_num;
	int time_send;
	int time_finish;
	int time_wait;
	int req_type;
	int req_place;
	//原始请求队列结构
};

struct Sta_stat {
	int stat_num;
	int stat_clkw;
	int stat_cclkw;
	int stat_tgt;
	int judgement;//添加judgement,1代表目标站，0代表顺便站，-1代表其他站
	int stat_sec;//时间戳1（最新的入队时间）
	int order_sec;//时间戳2（被确定为目标站的时间）
	int stat_distance;//对于目标站，保存它在被确定为目标站的那1s时与车的距离
	int stat_clkw_order;
	int stat_cclkw_order;
	int stat_tgt_order;

	//站点状态序列
};

struct Sta_bus {
	int bus_pos;
	int bus_dir;
	int bus_state;
	//bus状态
};

//字符串匹配
int str_match(const char *name_str[], int num, char buf[]) {
	//返回str_name中正确匹配buf的下标
	for (int i = 0; i < num; i++)
		if (strcmp(buf, name_str[i]) == 0)
			return i;
	//或逐字符比较也可
	return -1;
}

//配置参数
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
			//分离单词

			int num = str_match(name_param, Num_param, single_buf);
			//字符串匹配
			i += 3;
			//跳过等号（假定等号格式合法）
			//假定有效行内容保证合法
			//TOTAL_STATION在2~10
			//DISTANCE在1~5
			switch (num) {
				case 0:
					*total_station = atoi(&buffer[i]);
					break;
				//atoi函数 将等号之后的字符串转换为整数读入
				case 1: {
					int j = 0;
					while (buffer[i] != '\n')
						//fgets会读回车换行符
					{
						single_buf[j] = buffer[i];
						i++;
						j++;
					}
					single_buf[j] = 0;
					//梅开二度分离单词
					*strategy = str_match(name_strategy, Num_param, single_buf);
					//梅开二度字符串匹配
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

//读指令
int get_cmd() {
	char single_buf[Max_single_buf];
	scanf("%s", single_buf);
	return str_match(name_cmd, Num_cmd, single_buf);
}

//队列初始化
void que_init(Sta_req original[], Sta_stat station[], int total_station) {
	//数组均从下标1开始用
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
		station[i].judgement = -1;//初始化
		station[i].stat_sec = -1;
		station[i].order_sec = 1000000;
		station[i].stat_distance = 1000000;
		station[i].stat_clkw_order = 1000000;
		station[i].stat_cclkw_order = 1000000;
		station[i].stat_tgt_order = 1000000;
	}
}

//公交车状态机
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

//读取请求并入队
void queue_in(int cmd, int *cnt_tgt, int sec, Sta_req original[], Sta_stat station[], int order) {
	int tgt;
	scanf("%d", &tgt);
	//输入需保证指令后必须输一个数
	//输入需保证不会越界
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

//输出
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

//添加的函数1，用于确定目标站和顺便站
void dispatch(Sta_stat station[], int total_station, int total_distance, int distance, Sta_bus *bus, int sec) {
	int t = 0, a1 = 3, statsec = -1;
	for (int i = 1; i <= total_station; i++) {
		if (station[i].judgement == 1) {
			t = i;//寻找是否有目标站
			statsec = station[i].stat_sec;
			break;
		}
	}

	if (t != 0 && statsec == sec) {
		for (int i = 1; i <= total_station; i++);
	}





	if (t != 0) {//有目标站时，确定顺便站
		int k = ((*bus).bus_pos) / distance + 1;
		if ((*bus).bus_dir == CLKW) {//顺时针
			if (k < station[t].stat_num) {//分情况讨论站点在车的左或右
				for (int i = k + 1; i < station[t].stat_num; i++) {
					if (station[i].stat_clkw == 1 || station[i].stat_tgt == 1)
						station[i].judgement = 0;//确定为顺便站
				}
			} else if (k > station[t].stat_num) {
				for (int i = 1; i <= total_station; i++) {
					if ((i < t || i > k) && (station[i].stat_clkw == 1 || station[i].stat_tgt == 1))
						station[i].judgement = 0;//确定为顺便站
				}

			}
		} else if ((*bus).bus_dir == CCLKW) {//逆时针
			if (k > station[t].stat_num) {//分情况讨论站点在车的左或右
				for (int i = k + 1; i > station[t].stat_num; i--) {
					if (station[i].stat_cclkw == 1 || station[i].stat_tgt == 1)
						station[i].judgement = 0;//确定为顺便站
				}
			} else if (k < station[t].stat_num) {
				for (int i = 1; i <= total_station; i++) {
					if ((i <= k || i > t) && (station[i].stat_cclkw == 1 || station[i].stat_tgt == 1))
						station[i].judgement = 0;//确定为顺便站
				}
			}

		}
	}

	else if (t == 0) {//无目标站时，确定目标站
		int judg1[total_station + 3];//三个judg数组分别用于存储顺、逆和下车的车站与车位置差的绝对值
		int judg2[total_station + 3];
		int judg3[total_station + 3];
		for (int i = 1; i <= total_station; i++) {
			judg1[i] = 3000;
			judg2[i] = 3000;
			judg3[i] = 3000;
		}
		int stat1 = 0, stat2 = 0, stat3 = 0, min1 = 10000000, min2 = 10000000,
		    min3 = 10000000;//state存储站点号,min寻找三种情况下分别的最小值
		for (int i = 1; i <= total_station; i++) {
			if (station[i].stat_clkw == 1) {//判断第i站是否有顺时针请求
				if ((station[i].stat_num - 1) * distance >= (*bus).bus_pos) {
					judg1[i] = (station[i].stat_num - 1) * distance - (*bus).bus_pos;
				} else
					judg1[i] = total_distance - (*bus).bus_pos + (station[i].stat_num - 1) * distance;
			}
			if (station[i].stat_cclkw == 1) {//判断第i站是否有逆时针请求
				if ((station[i].stat_num - 1) * distance >= (*bus).bus_pos) {
					judg2[i] = (*bus).bus_pos + total_distance - (station[i].stat_num - 1) * distance;
				} else
					judg2[i] = (*bus).bus_pos - (station[i].stat_num - 1) * distance;
			}
			if (station[i].stat_tgt == 1) {//判断第i站是否有下车请求
				if ((station[i].stat_num - 1) * distance >= (*bus).bus_pos) {//下车请求分别寻找顺时针更近还是逆时针更近
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
		for (int i = 1; i <= total_station; i++) {//顺时针寻找最小距离及其站台
			if (min1 > judg1[i]) {
				min1 = judg1[i];
				stat1 = i;
			}
		}
		for (int i = 1; i <= total_station; i++) {//逆时针寻找最小距离及其站台
			if (min2 > judg2[i]) {
				min2 = judg2[i];
				stat2 = i;
			}
		}
		for (int i = 1; i <= total_station; i++) {//下车寻找最小距离及其站台
			if (min3 > judg3[i]) {
				min3 = judg3[i];
				stat3 = i;
			}
		}


		if (min1 < min2 && min1 < min3) {//顺时针最小，确定车行驶方向和目标站
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
		} else if (min2 < min1 && min2 < min3) {//逆时针最小，确定车行驶方向和目标站
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
		} else if (min3 < min1 && min3 < min2) {//下车最小，确定车行驶方向和目标站
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
//添加的函数2，当车行驶到站台位置时对站台进行操作
void station_operation(Sta_stat station[], int total_station, int total_distance, int distance,

                       int *stop_judg,//添加的变量，用来实现让车在站台停1s，主函数中解释实现原理
                       Sta_bus *bus, int sec) {
	int i = (*bus).bus_pos / distance + 1;//添加的变量，i=站台号
	switch (station[i].judgement) {
		case -1://其他站,无需操作
			break;
		case 0://顺便站，同方向及下车置0
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

			station[i].judgement = -1;//将顺便站改为其他站
			*stop_judg = 1;
			(*bus).bus_state = Stop;//将车的状态由Going改为Stop
			break;
		case 1:
			station[i].stat_tgt = 0;
			station[i].stat_clkw = 0;
			station[i].stat_cclkw = 0;
			station[i].stat_clkw_order = 1000000;
			station[i].stat_cclkw_order = 1000000;
			station[i].stat_tgt_order = 1000000;
			station[i].judgement = -1;//将目标站改为其他站
			*stop_judg = 1;
			(*bus).bus_state = Stop;//将车的状态由Going改为Stop
			dispatch(station, total_station, total_distance, distance, bus, sec); //立刻寻找下一个目标站
			break;
	}

}



//主函数
int main(void) {
	int total_station = 5;
	int strategy = FCFS;
	int distance = 2;
	int total_distance = total_station * distance;
	//此时:位置：0 1 2 3 4 5 6 7 8 9 (0)循环
	//     车站：1   2   3   4   5   (1)//位置=车站*2-2
	set_param(&total_station, &strategy, &distance, &total_distance); //配置参数
//	printf("%d %d %d %d\n",total_station,strategy,distance,total_distance);//测试参数配置

	Sta_req original[Max_buffer];//原始请求队列//长度为指定常量Max_buffer即1000//待修改，动态数组
	Sta_stat station[total_station + 1]; //站点状态序列//以变长数组声明了
	que_init(original, station, total_station); //队列初始化

	int sec = -1; //时间计数
	int cnt_tgt = 0; //请求计数
	Sta_bus bus = {0, CLKW, Start}; //bus的状态

	int order = 0;

	int stop_judg = 0;
	//添加的变量，用来实现让车在站台停1s。
	// 开始时置0，当车行驶至顺便站或目标站时，调用函数传地址将其改为1，车的状态改为Stop
	//进而避开后面的"若stop_judg等于0，遍历站点，若存在目标站则将车的状态改为Going"的功能1，以实现原地停靠1s。
	// 同时，为了让车仅仅停靠1s，在第二次循环的开始位置，插入功能2“若stop_judg等于1，遍历站点，
	// 若存在目标站则将stop_judg改为0”。这样再次运行到后面
	//时车的状态就可以改为Going,下一个clock就可以继续走了。

	int cmd = CLK; //便于0秒时的输出
	while (cmd != END) {
		if (cmd == CLK) {
			sec++;
			bus_run(&bus, total_distance); //公交车状态机
			info_prin(sec, bus, station, total_station); //输出
			if (stop_judg == 1) {//上文提到的功能2
				for (int i = 1; i <= total_station; i++) {
					if (station[i].judgement == 1) {
						stop_judg = 0;
						break;
					}
				}
			}
			if ((bus.bus_pos) % distance == 0) {//站点位置，调用函数来操作站点
				station_operation(station, total_station, total_distance, distance, &stop_judg, &bus, sec);
			}
		} else if (cmd == TGT || cmd == CLKW || cmd == CCLKW) {
			order++;
			queue_in(cmd, &cnt_tgt, sec, original, station, order); //读取请求并入队
			//待补充:)
			dispatch(station, total_station, total_distance, distance, &bus, sec); //对新入队的，判断站点的分类是否需要变更
		} else
			printf("invalid cmd name\n");

		if (stop_judg == 0) {//上文提到的功能1
			for (int i = 1; i <= total_station; i++) {
				if (station[i].judgement == 1) {
					bus.bus_state = Going;
					break;
				}
			}
		}
		cmd = get_cmd(); //读指令
	}
	printf("end\n");

//	system("pause");
	return 0;
}