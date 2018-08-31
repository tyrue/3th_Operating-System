#include <stdio.h>			
#include "pthread.h"    // ������ �������
#include <Windows.h>	// Sleep��
#define NUM_THREADS 5	// ö���� 5��

pthread_mutex_t mutexes[NUM_THREADS];		// ���ؽ� ����, ���� �ڿ� ������ ���� ���� �ð� ����� ����ȭ�� �޼��Ѵ�.
pthread_cond_t conditionVars[NUM_THREADS];	// �� �������� ���� ��, �������� �ʱ�ȭ�Ѵ�.

int permits[NUM_THREADS];					// ��ũ ��� ����, TRUE�� ��� ����
pthread_t tids[NUM_THREADS];				// ������ID�� �����Ͽ� �� �����带 �ĺ��Ѵ�, tids�迭�� ����

void pickup_forks(int philosopher_number)	// ��ũ�� ���� �Լ�, �Ӱ豸��(��ũ�� ���� ����)���� �ٸ� �����尡 �ݳ� �� ������ �����
{
	Sleep(10);
	pthread_mutex_lock(&mutexes[philosopher_number%NUM_THREADS]);	// �ش� ö���� ��ȣ(���� ��ũ)�� ���ؽ��� ��ٴ�. ��, �Ӱ豸�� ����. 
																	// ���� ��׷��� ���ؽ��� �ٸ� �����忡 ���ؼ� �̹� ����ִٸ�, 
																	// �̹� ��� �ٸ� �����尡 ���ؽ��� ����� �����Ҷ����� ����(��)�ǰ� �ȴ�. 
	while (permits[philosopher_number%NUM_THREADS] == FALSE)		// ���� ��ũ�� �̹� ���ǰ� ������ ����Ѵ�.
	{
		// �������� �����带 ��� �ߴܽ�Ų��. �ٸ� ������κ��� signal�̳� broadcast�� �޾��� ��� ����� mutex_lock�� �Ѵ�.
		pthread_cond_wait(&conditionVars[philosopher_number%NUM_THREADS], &mutexes[philosopher_number%NUM_THREADS]);
	} 
	permits[philosopher_number%NUM_THREADS] = FALSE;				// �ش� ���� ��ũ�� ��� �Ұ��� �����.
	pthread_mutex_unlock(&mutexes[philosopher_number%NUM_THREADS]);	// �����带 ������ ������ �����ϱ� ���� �ڽ��� ������ �ִ� mutex�� Ǯ���ش�.
																	// ���� �̸� �������� ������ �ٸ� ������� mutex�� Ǯ�� �� ���� ����ؾ���
}

void return_forks(int philosopher_number)							// ��ũ�� ���������Լ�, �Ӱ豸������ ����ϴ� �ٸ� �����带 Ǯ����
{
	Sleep(10);
	pthread_mutex_lock(&mutexes[philosopher_number%NUM_THREADS]);	// �ش� ��ȣ ���ؽ��� ��ٴ�.

	permits[philosopher_number%NUM_THREADS] = TRUE;					// �ش� ���� ��ũ�� ��� �����ϰ� �Ѵ�.
	pthread_cond_signal(&conditionVars[philosopher_number%NUM_THREADS]);// �ش� ���� ��ȣ(conditionVasrs)�� ������ �����(pthread_cond_wait)�� 
																// �ϳ��� �����带 �����. ���� thread�� �ټ��� ��� �� �ϳ��� thread�� �����.
	pthread_mutex_unlock(&mutexes[philosopher_number%NUM_THREADS]);	// ���ؽ��� Ǯ����
}

void* Philosopher(void * arg)		// ¦�� ö������ �Լ�, ���� ���� ���, �������� ���.
{
	int philosopher_number;			// ö���� �ĺ���
	philosopher_number = (int)arg;	// �ĺ��� �Ҵ�
	for (int i = 0; i < NUM_THREADS; i++)	// ö���� �� ��ŭ �Ե��� �Ѵ�.
	{
		// ���� ��ũ�� ����
		pickup_forks(philosopher_number);
		printf("philosopher(%d) picks up the left fork(%d).\n", philosopher_number, philosopher_number);

		// ������ ��ũ�� ����
		pickup_forks(philosopher_number + 1);
		printf("philosopher(%d) picks up the right fork(%d).\n", philosopher_number, (philosopher_number + 1) % NUM_THREADS);

		printf("philosopher(%d) starts eating(%d) \n\n", philosopher_number, i+1); // �� �� �Ծ����� Ȯ��
		Sleep(40);	// �Ļ���..
		printf("philosopher(%d) finishes eating \n", philosopher_number);

		// ������ ��ũ ��������
		return_forks(philosopher_number + 1);
		printf("philosopher(%d) put down the right fork(%d).\n", philosopher_number, (philosopher_number + 1) % NUM_THREADS);

		// ���� ��ũ ��������
		return_forks(philosopher_number);
		printf("philosopher(%d) put down the left fork(%d).\n", philosopher_number, philosopher_number);
	}
	return NULL;	// ������ ����
}

void * OddPhilosopher(void * arg) // Ȧ�� ö������ �Լ�, ������ ��ũ�� ���� ����, ���� ��ũ�� ���´�.
{
	int philosopher_number;
	philosopher_number = (int)arg;
	for (int i = 0; i < NUM_THREADS; i++) // ö���� �� ��ŭ �Ե��� �Ѵ�.
	{
		// ������ ��ũ�� ����
		pickup_forks(philosopher_number + 1);
		printf("philosopher(%d) picks up the right fork(%d).\n", philosopher_number, (philosopher_number + 1) % NUM_THREADS);

		// ���� ��ũ�� ����
		pickup_forks(philosopher_number);
		printf("philosopher(%d) picks up the left fork(%d).\n", philosopher_number, philosopher_number);

		printf("philosopher(%d) starts eating(%d) \n\n", philosopher_number, i+1); // �� �� �Ծ����� Ȯ��.
		Sleep(40);	// �Ļ���....
		printf("philosopher(%d) finishes eating \n", philosopher_number);

		// ���� ��ũ�� ��������
		return_forks(philosopher_number);
		printf("philosopher(%d) puts down the left fork(%d).\n", philosopher_number, philosopher_number);

		// ������ ��ũ�� ��������
		return_forks(philosopher_number + 1);
		printf("philosopher(%d) puts down the right fork(%d).\n", philosopher_number, (philosopher_number + 1) % NUM_THREADS);
	}
	return NULL;	// ������ ����
}

int main() 
{
	int i;

	for (i = 0; i < NUM_THREADS; i++) 
	{
		pthread_mutex_init(&mutexes[i], NULL);		// ���ؽ� ����
		pthread_cond_init(&conditionVars[i], NULL); // ���¸� NULL�� �ʱ�ȭ�Ѵ�.
		permits[i] = TRUE;							// ��� ��ũ�� ��Ź ���� �ִ�.
	}

	for (i = 0; i < NUM_THREADS; i++) 
	{
		if (i % 2) 
			pthread_create(&tids[i], NULL, OddPhilosopher, (void*)(i)); // Ȧ�� ������(ö����) ����, Ȧ��ö���� �Լ��� ��� �����ϰ� �ִ�.
		
		else 
			pthread_create(&tids[i], NULL, Philosopher, (void*)(i));	// ¦�� ������(ö����) ����, ¦��ö���� �Լ��� ��� �����ϰ� �ִ�.
	}

	for (i = 0; i < NUM_THREADS; i++) 
		pthread_join(tids[i], NULL); // ���� ������(ö���� ������)�� ���� �� ������ ���ν�����(main)�� ��� �ϵ��� �ϴ� �Լ�.
									 // 1 : � �����带 ��ٸ� �� ���ϴ� �ĺ���, 2 : �������� return ��, ����Ʈ�� ���� �޾ƿ��� ���� ����.

	for (i = 0; i < NUM_THREADS; i++)
	{
		pthread_mutex_destroy(&mutexes[i]);			// �����带 �ı�. 
		pthread_cond_destroy(&conditionVars[i]);	// ���°� �ı�.
	}
	return 0;
}