#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include <io.h>
#include<windows.h>

#pragma warning(disable:4996);								//消除误报错

typedef const char cchar;

typedef struct {
	int pixel[1024];
	int label;
}Digit;

typedef struct {
	float distance;
	int label;
}Distance;

//文件路径+名称
//如果想测试，必须改路径，改成在接受的数据文件夹的路径				第139，175，240，275行路径同样需要更改
cchar *trainingFile = "D:\\code\\C\\schoolbookstudy\\match2\\trainingDigits\\*.txt";				//训练数据的路径
cchar *testFile = "D:\\code\\C\\schoolbookstudy\\match2\\testDigits\\*.txt";						//测试数据的路径
cchar *predictFile = "D:\\code\\C\\schoolbookstudy\\match2\\predictDigits\\*.txt";				//想要测试数据的路径


float calDistance(Digit digit1, Digit digit2)
{													//两个数据集的距离计算，采用欧式距离计算
	int i, squareSum = 0.0;
	for (i = 0;i < 1024;i++)
		squareSum += pow(digit1.pixel[i] - digit2.pixel[i], 2.0);
	return sqrtf(squareSum);						
}

int loadDigit(Digit *digit, FILE*fp, int *labels)
{													//将读入的训练和测试数据集存入digit里面
	int index = 0;
	for (index = 0;index < 1024;index++) 
	{
		if (!fscanf(fp, "%1d", &(digit->pixel[index])))
		{
			printf("FILE already read finish.\n");
			return -1;
		}
	}
	fscanf(fp, "%d", &(digit->label));
	*labels = digit->label;
	fclose(fp);
	return 1;
}

int loadDigit2(Digit *digit, FILE*fp) 
{													//将读入的预测数据集存入digit里面
	int index = 0;
	for (index = 0;index < 1024;index++) 
	{
		if (!fscanf(fp, "%1d", &(digit->pixel[index])))
		{
			printf("FILE already read finish.\n");
			return -1;
		}
	}
	fclose(fp);
	return 1;
}

void showDigit(Digit digit) 
{													//将数据集打印出来的函数
	int i, j, id;
	for (i = 0;i < 32;i++) 
	{
		for (j = 0;j < 32;j++) 
		{
			printf("%d", digit.pixel[i * 32 + j]);
		}
		printf("\n");
	}

}

void swap(Distance *in, int index1, int index2) 
{													//交换两个值的函数，用于排序
	Distance tmp = in[index1];
	in[index1] = in[index2];
	in[index2] = tmp;
}


void selectSort(Distance *in, int length) 
{													//选择排序法
	int i, j, min;
	int N = length;
	for (i = 0;i < N - 1;i++) 
	{
		min = i;
		for (j = i + 1;j < N;j++) 
		{
			if (in[j].distance < in[min].distance)
				min = j;
		}
		swap(in, i, min);
	}
}


int prediction(int K, Digit in, Digit *train, int nt) 
{																//预测k值函数
	int i, it;
	Distance distance[1934];
	//求取输入digit与训练数据的距离
	for (it = 0;it < nt;it++)
	{
		distance[it].distance = calDistance(in, train[it]);
		distance[it].label = train[it].label;
	}
	//给计算的距离排序（选择排序）	
	int predict = 0;												//距离从小到大排序后，前K个数的类型就最有可能是要预测的那个数
	selectSort(distance, nt);
	for (i = 0;i < K;i++)
	{
		predict += distance[i].label;
	}
	
	return (int)(predict / K);

}


void knn_classifiy(int K) {											//此函数读入测试数据和训练数据，用测试数据中的每一个与训练数据中的每一个进行距离测算，再使用prediction预测函数得出测试数据的预测值，
	printf("knn_classifiy...\n");									//然后使用这个预测值和测试数据本身的数字类型对比，如果相同则该数字的预测成功次数加一，最后再得出每个数字预测成功的成功率，从而根据
																	//成功率的大小来确定最佳K值。			经过测试当  K = 1 时成功率最大。

																	//读入训练数据
	printf("loading training digits...\n");							//使用findfirst和findnext进行训练数据文件夹下的每个训练数据读入
	int i = 0;
	FILE *fp;
	struct _finddata_t fileinfo;
	long sign;
	char file_str[2048];
	int trainLabels[2048];
	int trainCount[10] = { 0 };
	Digit *Dtrain = (Digit*)malloc(2048 * sizeof(Digit));
	if ((sign = _findfirst(trainingFile, &fileinfo)) == -1L)
	{
		printf("open file fail\n");
		return 0;
	}
	do
	{
		sprintf(file_str, "D:\\code\\C\\schoolbookstudy\\match2\\trainingDigits\\%s", fileinfo.name);				//需要测试得更改路径
		if ((fp = fopen(file_str, "r")) == -1)
		{
			printf("open file fail\n");
		}

		else
		{
			printf("open file trainingDigit:%s success\n", fileinfo.name);
			loadDigit(&Dtrain[i], fp, &trainLabels[i]);
			trainCount[Dtrain[i].label]++;
			i++;
			fclose(fp);
		}
	} while (_findnext(sign, &fileinfo) == 0);
	_findclose(sign);
	printf("\n");


	//读入测试数据
	printf("loading test digits...\n");								//使用findfirst和findnext进行测试数据文件夹下的每个训练数据读入
	i = 0;
	FILE *fp1;
	struct _finddata_t fileinfo2;
	long sign1;
	char file_str1[1024];
	int testLabels[1024];
	int testCount[10] = { 0 };
	Digit *Dtest = (Digit*)malloc(1024 * sizeof(Digit));
	if ((sign1 = _findfirst(testFile, &fileinfo2)) == -1L)
	{
		printf("open file fail\n");
		return 0;
	}
	do
	{
		sprintf(file_str1, "D:\\code\\C\\schoolbookstudy\\match2\\testDigits\\%s", fileinfo2.name);				//需要测试得更改路径
		if ((fp1 = fopen(file_str1, "r")) == -1)
		{
			printf("open file fail\n");
		}

		else
		{
			printf("open file testDigit:%s success\n", fileinfo2.name);
			loadDigit(&Dtest[i], fp1, &testLabels[i]);
			testCount[Dtest[i].label]++;
			i++;
			fclose(fp1);
		}
	} while (_findnext(sign1, &fileinfo2) == 0);
	_findclose(sign1);

	printf("Calculating Distance begin...\n");
	int CorrectCount[10] = { 0 };									//用于记录每个数字预测成功的次数
	int itrain, itest, predict;

	for (itest = 0; itest<946; itest++)
	{
		predict = prediction(1, Dtest[itest], Dtrain, 1934);		//进行预测，得到预测值
																	//给预测准确的进行计数
		if (predict == Dtest[itest].label)
		{
			CorrectCount[predict] += 1;
		}
	}

	//输出测试数据的正确率
	printf("Correct radio:\n\n");
	system("cls");
	for (i = 0;i<10;i++)
	{
		printf("%d:  (  %2d / %2d ) =  %.2f%%\n", i, CorrectCount[i], testCount[i], (float)(CorrectCount[i] * 1.0 / testCount[i] * 100));
	}
	printf("\n\n\n");
}


void knn_predict(int K) {
	Sleep(5000);										//便于观察正确率
	//读入训练数据
	printf("loading training digits...\n");				//使用findfirst和findnext进行训练数据文件夹下的每个训练数据读入
	int i = 0;
	FILE *fp;
	struct _finddata_t fileinfo;
	long sign;
	char file_str[2048];
	int trainLabels[2048];
	int trainCount[10] = { 0 };
	Digit *Dtrain = (Digit*)malloc(2048 * sizeof(Digit));
	if ((sign = _findfirst(trainingFile, &fileinfo)) == -1L)
	{
		printf("open file fail\n");
		return 0;
	}
	do
	{
		sprintf(file_str, "D:\\code\\C\\schoolbookstudy\\match2\\trainingDigits\\%s", fileinfo.name);				//需要测试得更改路径
		if ((fp = fopen(file_str, "r")) == -1)
		{
			printf("open file fail\n");
		}

		else
		{
			printf("open file trainingDigit:%s success\n", fileinfo.name);
			loadDigit(&Dtrain[i], fp, &trainLabels[i]);
			trainCount[Dtrain[i].label]++;
			i++;
			fclose(fp);
		}
	} while (_findnext(sign, &fileinfo) == 0);
	_findclose(sign);
	printf("\n");

	////读入要测的数据							//使用findfirst和findnext进行想要预测数据文件夹下的每个数据读入
	i = 0;
	FILE *fp1;
	struct _finddata_t fileinfo2;
	long sign1;
	char file_str1[1024];
	int predictLabels[1024];
	Digit *Dpredict = (Digit*)malloc(1024 * sizeof(Digit));
	if ((sign1 = _findfirst(predictFile, &fileinfo2)) == -1L)
	{
		printf("open file fail\n");
		return 0;
	}
	do
	{
		sprintf(file_str1, "D:\\code\\C\\schoolbookstudy\\match2\\predictDigits\\%s", fileinfo2.name);					//需要测试得更改路径
		if ((fp1 = fopen(file_str1, "r")) == -1)
		{
			printf("open file fail\n");
		}

		else
		{
			printf("open predictfile predictDigit %s success\n", fileinfo2.name);
			loadDigit2(&Dpredict[i], fp1);
			i++;
			fclose(fp1);
		}
	} while (_findnext(sign1, &fileinfo2) == 0);
	_findclose(sign1);
	int t = i;
	printf("\n\n\n");
	int predictNum;
	for (i = 0;i < t;i++) 
	{
		showDigit(Dpredict[i]);														//显示出每个手写数字，便于与预测数据对比
		predictNum = prediction(1, Dpredict[i], Dtrain, 1934);
		printf("The number could be:%d\n\n\n\n", predictNum);
	}
}

int main()
{
	knn_classifiy(1);
	knn_predict(1);
	system("pause");
}