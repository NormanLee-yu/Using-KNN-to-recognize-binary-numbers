#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include <io.h>
#include<windows.h>

#pragma warning(disable:4996);								//�����󱨴�

typedef const char cchar;

typedef struct {
	int pixel[1024];
	int label;
}Digit;

typedef struct {
	float distance;
	int label;
}Distance;

//�ļ�·��+����
//�������ԣ������·�����ĳ��ڽ��ܵ������ļ��е�·��				��139��175��240��275��·��ͬ����Ҫ����
cchar *trainingFile = "D:\\code\\C\\schoolbookstudy\\match2\\trainingDigits\\*.txt";				//ѵ�����ݵ�·��
cchar *testFile = "D:\\code\\C\\schoolbookstudy\\match2\\testDigits\\*.txt";						//�������ݵ�·��
cchar *predictFile = "D:\\code\\C\\schoolbookstudy\\match2\\predictDigits\\*.txt";				//��Ҫ�������ݵ�·��


float calDistance(Digit digit1, Digit digit2)
{													//�������ݼ��ľ�����㣬����ŷʽ�������
	int i, squareSum = 0.0;
	for (i = 0;i < 1024;i++)
		squareSum += pow(digit1.pixel[i] - digit2.pixel[i], 2.0);
	return sqrtf(squareSum);						
}

int loadDigit(Digit *digit, FILE*fp, int *labels)
{													//�������ѵ���Ͳ������ݼ�����digit����
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
{													//�������Ԥ�����ݼ�����digit����
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
{													//�����ݼ���ӡ�����ĺ���
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
{													//��������ֵ�ĺ�������������
	Distance tmp = in[index1];
	in[index1] = in[index2];
	in[index2] = tmp;
}


void selectSort(Distance *in, int length) 
{													//ѡ������
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
{																//Ԥ��kֵ����
	int i, it;
	Distance distance[1934];
	//��ȡ����digit��ѵ�����ݵľ���
	for (it = 0;it < nt;it++)
	{
		distance[it].distance = calDistance(in, train[it]);
		distance[it].label = train[it].label;
	}
	//������ľ�������ѡ������	
	int predict = 0;												//�����С���������ǰK���������;����п�����ҪԤ����Ǹ���
	selectSort(distance, nt);
	for (i = 0;i < K;i++)
	{
		predict += distance[i].label;
	}
	
	return (int)(predict / K);

}


void knn_classifiy(int K) {											//�˺�������������ݺ�ѵ�����ݣ��ò��������е�ÿһ����ѵ�������е�ÿһ�����о�����㣬��ʹ��predictionԤ�⺯���ó��������ݵ�Ԥ��ֵ��
	printf("knn_classifiy...\n");									//Ȼ��ʹ�����Ԥ��ֵ�Ͳ������ݱ�����������ͶԱȣ������ͬ������ֵ�Ԥ��ɹ�������һ������ٵó�ÿ������Ԥ��ɹ��ĳɹ��ʣ��Ӷ�����
																	//�ɹ��ʵĴ�С��ȷ�����Kֵ��			�������Ե�  K = 1 ʱ�ɹ������

																	//����ѵ������
	printf("loading training digits...\n");							//ʹ��findfirst��findnext����ѵ�������ļ����µ�ÿ��ѵ�����ݶ���
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
		sprintf(file_str, "D:\\code\\C\\schoolbookstudy\\match2\\trainingDigits\\%s", fileinfo.name);				//��Ҫ���Եø���·��
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


	//�����������
	printf("loading test digits...\n");								//ʹ��findfirst��findnext���в��������ļ����µ�ÿ��ѵ�����ݶ���
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
		sprintf(file_str1, "D:\\code\\C\\schoolbookstudy\\match2\\testDigits\\%s", fileinfo2.name);				//��Ҫ���Եø���·��
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
	int CorrectCount[10] = { 0 };									//���ڼ�¼ÿ������Ԥ��ɹ��Ĵ���
	int itrain, itest, predict;

	for (itest = 0; itest<946; itest++)
	{
		predict = prediction(1, Dtest[itest], Dtrain, 1934);		//����Ԥ�⣬�õ�Ԥ��ֵ
																	//��Ԥ��׼ȷ�Ľ��м���
		if (predict == Dtest[itest].label)
		{
			CorrectCount[predict] += 1;
		}
	}

	//����������ݵ���ȷ��
	printf("Correct radio:\n\n");
	system("cls");
	for (i = 0;i<10;i++)
	{
		printf("%d:  (  %2d / %2d ) =  %.2f%%\n", i, CorrectCount[i], testCount[i], (float)(CorrectCount[i] * 1.0 / testCount[i] * 100));
	}
	printf("\n\n\n");
}


void knn_predict(int K) {
	Sleep(5000);										//���ڹ۲���ȷ��
	//����ѵ������
	printf("loading training digits...\n");				//ʹ��findfirst��findnext����ѵ�������ļ����µ�ÿ��ѵ�����ݶ���
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
		sprintf(file_str, "D:\\code\\C\\schoolbookstudy\\match2\\trainingDigits\\%s", fileinfo.name);				//��Ҫ���Եø���·��
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

	////����Ҫ�������							//ʹ��findfirst��findnext������ҪԤ�������ļ����µ�ÿ�����ݶ���
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
		sprintf(file_str1, "D:\\code\\C\\schoolbookstudy\\match2\\predictDigits\\%s", fileinfo2.name);					//��Ҫ���Եø���·��
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
		showDigit(Dpredict[i]);														//��ʾ��ÿ����д���֣�������Ԥ�����ݶԱ�
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