#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LEN_MIN 15  // ���� �ּ� ����
#define LEN_MAX 50  // ���� �ִ� ����
#define PROB_MIN 10 // �ּ� Ȯ��
#define PROB_MAX 90 // �ִ� Ȯ��
// �ùΰ� ���� ��Ÿ���� ���� ���
#define CITIZEN 'C'
#define ZOMBIE 'Z'
#define EMPTY_SPACE ' '
#define MADONGSEOK 'M'
// ������ ü��
#define STM_MIN 0
#define STM_MAX 5
// ���� �̵� ����
#define MOVE_LEFT 1
#define MOVE_STAY 0
// ���� �ൿ
#define ACTION_REST 0
#define ACTION_PROVOKE 1
#define ACTION_PULL 2
// ��׷�
#define AGGRO_MIN 1
#define AGGRO_MAX 5
// ���� ���� ���
#define ATK_NONE 0
#define ATK_CITIZEN 1
#define ATK_DONGSEOK 2

char train[LEN_MAX];

int bm_aggro = 1; // ������ ���� ��׷� ��ġ ����
int bc_aggro = 1; // ��� ���� ��׷� ��ġ ����
int b_zb;         // ���� ���� ������ ���п�
int b_stmaina;    // ������ ���� ü��

// ���� �ʱ� ���� �Լ�
void initializeTrain(int train_length)
{
    // �����ʺ��� M, Z, �� ĭ 2��, C ��ġ�ؾ���

    for (int i = 0; i < train_length; i++)
    {
        if (i == train_length - 2)
            train[i] = MADONGSEOK;
        else if (i == train_length - 3)
            train[i] = ZOMBIE;
        else if (i >= train_length)
            train[i] = EMPTY_SPACE;
        else if (i == train_length - 6)
            train[i] = CITIZEN;
        else if (i == 0)
            train[i] = '#';
        else if (i == train_length - 1)
            train[i] = '#';
        else
            train[i] = EMPTY_SPACE;
    }
}

// ���� ���� ��� �Լ�
void printTrain(int length)
{
    // ���
    for (int i = 0; i < length; i++)
    {
        printf("#");
    }
    printf("\n");
    // ���� ����
    for (int i = 0; i < length; i++)
    {
        printf("%c", train[i]);
    }
    printf("\n");
    // �ϴ�
    for (int i = 0; i < length; i++)
    {
        printf("#");
    }
    printf("\n");
}

// ����� �������� �й��� ��
void citizen_and_ma_lose(int* mst, int train_length)
{
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == ZOMBIE)
        {
            if (i == CITIZEN)
            {
                printf("�ù��� ���񿡰� ���Ƚ��ϴ�. �ٽ� �����غ����� !\n");
                exit(0);
            }
        }
    }
    if (*mst == STM_MIN)
    {
        printf("�������� ���������ϴ�... �ٽ� �����غ����� !\n");
        exit(0);
    }
    return;
}

// ����� �ൿ, �̱�ų� �Ѿ�ų�
void citizen_action(int train_length)
{

    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == CITIZEN)
        {
            if (i == 1)
            {
                printf("���ϵ帳�ϴ�. �ù��� �λ��� �������� Ż���߽��ϴ� !\n");
                exit(0);
            }
        }
    }
    printf("�ù��� �ƹ��͵� ���� �ʽ��ϴ�.\n");
}

// �ù� �̵� �Լ� ��׷� ��ġ �߰�,
void moveCitizen(double probability, int train_length, int* moved_citizen, int* c_aggro)
{
    bc_aggro = *c_aggro;
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == CITIZEN)
        {
            if (i == 1)
            {
                printf("���ϵ帳�ϴ�. �ù��� �λ��� �������� Ż���߽��ϴ� !\n");
                exit(0);
            }
            else if (i > 0)
            {
                double move_probability = ((double)rand() / RAND_MAX);
                if (move_probability < (1 - (probability / 100.0)))
                {
                    train[i - 1] = train[i];
                    train[i] = EMPTY_SPACE;
                    *moved_citizen = 1;
                    (*c_aggro)++; // �ù� ��׷� ����
                    return;
                }
                else
                {
                    *moved_citizen = 0;
                    (*c_aggro)--; // �ù� ��׷� ����
                    return;
                }
            }
        }
    }
    *moved_citizen = 0; // �̵����� �ʾ��� ���
}

// ���� �̵� �Լ� ¦�� �Ͽ��� �̵� �����ϰ�, �������� �������� �������̰�, ���� ����� ����� ã�ų� ���ٸ� ��׷� ��ġ�� ���� ������ �̵��ϸ�, ��׷ΰ� ������ ��� �ù������� �̵�.
void moveZombie(double probability, int train_length, int* moved_zombie, int turn, int* zombie_held, int* c_aggro, int* m_aggro)
{
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == ZOMBIE)
        {
            b_zb = i;
            if (*zombie_held == 1)
            {
                printf("���� �ٵ���� �̵��� �� �����ϴ�.\n");
                *zombie_held = 0;
                return;
            }

            if (turn % 2 == 0)
            { // ¦�� �Ͽ� �̵�
                *moved_zombie = 1;
                return;
            }

            // ���� ����� ���(�ù� �Ǵ� ������)�� ã��
            int target_index = -1;
            int closest_aggro = -1; // ���� ���� ��׷� ��ġ
            for (int j = 0; j < train_length; j++)
            {
                if (train[j] == CITIZEN || train[j] == MADONGSEOK)
                {
                    int current_aggro = (train[j] == CITIZEN) ? *c_aggro : *m_aggro;
                    if (current_aggro > closest_aggro)
                    {
                        closest_aggro = current_aggro;
                        target_index = j;
                    }
                    else if (current_aggro == closest_aggro && train[j] == CITIZEN)
                    {
                        // ��׷� ��ġ�� ������ �ù� ������ �̵�
                        target_index = j;
                    }
                }
            }

            // ����� �������� ���� ��쿡�� �̵�
            if (target_index != -1)
            {
                if (target_index > i && train[i + 1] == EMPTY_SPACE && train[i + 1] != MADONGSEOK)
                {
                    // ������(������)���� �̵� ������ ��� �̵�
                    train[i + 1] = ZOMBIE;
                    train[i] = EMPTY_SPACE;
                    *moved_zombie = 1;
                    return;
                }
                else if (target_index < i && train[i - 1] == EMPTY_SPACE && train[i - 1] != CITIZEN)
                {
                    // �ù�(����)���� �̵� ������ ��� �̵�
                    train[i - 1] = ZOMBIE;
                    train[i] = EMPTY_SPACE;
                    *moved_zombie = 1;
                    return;
                }
            }
        }
    }
    *moved_zombie = 0; // �̵����� �ʾ��� ���
}

// �ù� ���� ��� + ��׷� �߰�
void printCitizenStatus(int train_length, int moved_citizen, int turn, int* c_aggro)
{
    int citizen_position = -1;

    // �ù��� ��ġ ã��
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == CITIZEN)
        {
            citizen_position = i;
            break;
        }
    }

    // �ù� ���� ���
    if (moved_citizen == 0)
    {
        if (*c_aggro < AGGRO_MIN) //�ù� ��׷� �ּ� ��ġ ����
        {
            *c_aggro = AGGRO_MIN;
            printf("\n�ù�: stay %d ��׷� : %d -> %d \n", citizen_position, bc_aggro, *c_aggro);
        }
        else
        {
            printf("\n�ù�: stay %d ��׷� : %d -> %d \n", citizen_position, bc_aggro, *c_aggro);
        }
    }
    else
    {
        if (*c_aggro > AGGRO_MAX) //�ù� ��׷� �ִ� ��ġ ����
        {
            *c_aggro = AGGRO_MAX;
            printf("\n�ù�: %d �� %d ��׷� : %d -> %d \n", citizen_position + 1, citizen_position, bc_aggro, *c_aggro);
        }
        else
        {
            printf("\n�ù�: %d �� %d ��׷� : %d -> %d \n", citizen_position + 1, citizen_position, bc_aggro, *c_aggro);
        }
    }
}

// ���� ���� ��� �Լ�
void printZombieStatus(int train_length, int moved_zombie, int turn)
{
    int zombie_position = -1;

    // ������ ��ġ ã��
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == ZOMBIE)
        {
            zombie_position = i;
            break;
        }
    }

    // ���� ���� ���
    if (turn % 2 != 0)
    {
        if (zombie_position > b_zb)
        {
            printf("����: %d -> %d\n", b_zb, zombie_position);
        }
        else if (zombie_position < b_zb)
        {
            printf("����: %d -> %d\n", b_zb, zombie_position);
        }
    }
    else if (turn % 2 == 0)
    {
        printf("����: stay %d (�������� �ʴ� ���Դϴ�.)\n", zombie_position);
    }
    printf("\n");
}

// ������ �̵� �Լ� ����� ������ ������ ��⸸ �߰�, ���ٸ� �̵� ����,
void moveMadongseok(int train_length, int* moved_madongseok, int* m_aggro)
{
    int madongseok_position = -1;
    int move;
    int can_move_left = 1; // �������� �̵� ���� ����
    bm_aggro = *m_aggro;
    // �������� ���� ��ġ ã��
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == MADONGSEOK)
        {
            madongseok_position = i;
            break;
        }
    }

    // �������� �¿쿡 ���� �ִ��� Ȯ���ϱ�
    if ((madongseok_position > 0 && train[madongseok_position - 1] == ZOMBIE) ||
        (madongseok_position < train_length - 1 && train[madongseok_position + 1] == ZOMBIE))
    {
        can_move_left = 0;
    }

    // �̵� �Է¹ޱ�
    do
    {
        printf("�������� �̵��� �����ϼ���:\n");
        printf("0: ���ڸ� ���\n");
        if (can_move_left)
        {
            printf("1: �������� �̵�\n");
        }
        scanf_s("%d", &move);

        if (move != MOVE_STAY && move != MOVE_LEFT)
        {
            printf("�߸��� �Է��Դϴ�. �ٽ� �õ��ϼ���.\n");
        }
        else if (move == MOVE_LEFT && !can_move_left)
        {
            printf("�������� �̵��� �� �����ϴ�. �ٽ� �õ��ϼ���.\n");
        }

    } while ((move != MOVE_STAY && move != MOVE_LEFT) || (move == MOVE_LEFT && !can_move_left));

    if (move == MOVE_STAY)
    {
        // ���ڸ� ����ϱ�
        if (*m_aggro > AGGRO_MIN) // ��׷� ���̰�, �ּ� ��ġ ���� ����
        {
            (*m_aggro) -= 1;
        }
        else
        {
            *m_aggro = AGGRO_MIN;
        }
        *moved_madongseok = 0;
    }
    else if (move == MOVE_LEFT && can_move_left)
    {
        if (madongseok_position > 0 && train[madongseok_position - 1] == EMPTY_SPACE)
        {
            train[madongseok_position - 1] = MADONGSEOK;
            train[madongseok_position] = EMPTY_SPACE;
            *moved_madongseok = 1;
            if (*m_aggro >= AGGRO_MAX) // ��׷� �߰��ϰ�, �ִ� ��ġ ���� ����
            {
                *m_aggro = AGGRO_MAX;
            }
            else
            {
                (*m_aggro)++;
            }
        }
        else
        {
            printf("�������� �̵��� �� �����ϴ�.\n");
            *moved_madongseok = 0;
        }
    }
}
// ������ ���� ����ϱ�, ���¹̳�, ��׷� ��ġ ����
void printMadongseok(int train_length, int moved_madongseok, int* madongseok_stamina, int* m_aggro)
{
    int madongseok_position = -1;
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == MADONGSEOK)
        {
            madongseok_position = i; // ���� ��ġ ã��
            break;
        }
    }
    if (*madongseok_stamina == STM_MIN)
    {
        printf("�������� ���������ϴ�... �ٽ� �����غ����� !\n"); // ������ ���¹̳� �پ�� ��
        exit(0);
    }
    if (moved_madongseok == 1)
    {
        printf("\n������: ��ġ %d -> %d (��׷�: %d -> %d, ü��: %d)\n", madongseok_position + 1, madongseok_position, bm_aggro, *m_aggro, *madongseok_stamina);
    }
    else if (moved_madongseok == 0)
    {
        printf("\n ������: stay %d (��׷�: %d -> %d, ü��: %d)\n", madongseok_position, bm_aggro, *m_aggro, *madongseok_stamina);
    }
}

//���� �ൿ, �� �ʿ� ��� �� �������� ������ ���� ��쿡 �����ϰ�, �� �ʿ� �������ִٸ� ��׷� ��ġ�� ���� ������ ����, �ƹ��� ������ �Ѿ
void zombieAction(int train_length, int turn, int* madongseok_stamina, int* c_aggro, int* m_aggro)
{
    int b_stamina = *madongseok_stamina; // ���¹̳� ��ȭ �� ��ġ ����
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == ZOMBIE)
        {
            int left_target_index = -1;  // ���� ���� ��� ��ġ
            int right_target_index = -1; // ������ ���� ��� ��ġ

            // ���� ������ ��� Ȯ��
            if (i > 0 && (train[i - 1] == CITIZEN || train[i - 1] == MADONGSEOK))
            {
                left_target_index = i - 1;
            }
            // ������ ������ ��� Ȯ��
            if (i < train_length - 1 && (train[i + 1] == CITIZEN || train[i + 1] == MADONGSEOK))
            {
                right_target_index = i + 1;
            }

            // ���ʿ� �ù� �Ǵ� �������� ���� ��쿡 ������.. ��ǻ� �ùθ�...
            if (left_target_index != -1 && right_target_index == -1)
            {
                if (train[left_target_index] == CITIZEN)
                {
                    printf("�ù��� ���񿡰� ���� �޾Ƽ� �׾����ϴ�.");
                    exit(0);
                }
                else if (train[left_target_index] == MADONGSEOK)
                {
                    *madongseok_stamina -= 1;
                    printf("�������� ���񿡰� ���ݴ��߽��ϴ�. ü��: %d -> %d\n", b_stamina, *madongseok_stamina);

                    if (*madongseok_stamina == STM_MIN)
                    {
                        printf("�������� ���������ϴ�... �ٽ� �����غ����� !\n"); // �й�
                        exit(0);
                    }
                }
            }
            // �����ʿ� �ù� �Ǵ� �������� ���� ��쿡 ����. .. ���⵵ ��ǻ� ��������..
            else if (left_target_index == -1 && right_target_index != -1)
            {
                if (train[right_target_index] == CITIZEN)
                {
                    printf("�ù��� ���񿡰� ���� �޾Ƽ� �׾����ϴ�.");
                    exit(0);
                }
                else if (train[right_target_index] == MADONGSEOK)
                {
                    *madongseok_stamina -= 1;
                    printf("�������� ���񿡰� ���ݴ��߽��ϴ�. ü��: %d -> %d\n", b_stamina, *madongseok_stamina);

                    if (*madongseok_stamina == STM_MIN)
                    {
                        printf("�������� ���������ϴ�... �ٽ� �����غ����� !\n"); // �й�
                        exit(0);
                    }
                }
            }
            // ���� �� ������ ��� ��׷ΰ� ���� ���� ����
            else if (left_target_index != -1 && right_target_index != -1)
            {
                int target_index = (*c_aggro > *m_aggro) ? left_target_index : right_target_index;

                if (train[target_index] == CITIZEN)
                {
                    printf("�ù��� ���񿡰� ���� �޾Ƽ� �׾����ϴ�.");
                    exit(0);
                }
                else if (train[target_index] == MADONGSEOK)
                {
                    *madongseok_stamina -= 1;
                    printf("�������� ���񿡰� ���ݴ��߽��ϴ�. ��׷�: %d vs. %d, ü��: %d -> %d\n", *c_aggro, *m_aggro, b_stamina, *madongseok_stamina);

                    if (*madongseok_stamina == STM_MIN)
                    {
                        printf("�������� ���������ϴ�... �ٽ� �����غ����� !\n"); // �й�
                        exit(0);
                    }
                }
            }
            // �ƹ��� ������ �Ѿ .
            printf("������ ��� �����ϴ�.\n");
        }
    }
}

//������ �ൿ. ����� �������� �ʴٸ� �޽İ� ���߸�, ������ �ִٸ� ����� ����, �������� ���� ��������.
void madongseokAction(int train_length, int* madongseok_stamina, int* zombie_held, int probability, int* m_aggro)
{
    int action;
    int madongseok_position = -1;
    b_stmaina = *madongseok_stamina;
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == MADONGSEOK)
        {
            madongseok_position = i; // ��ġ�� �ּҸ� �Ҵ�
            break;
        }
    }

    // ����� �������� �ʴٸ� �޽�, ����
    if (madongseok_position > 0 && madongseok_position < train_length - 1 &&
        train[madongseok_position - 1] != ZOMBIE && train[madongseok_position + 1] != ZOMBIE)
    {
        printf("�������� �ൿ�� �����ϼ���:\n");
        printf("0: �޽�\n");
        printf("1: ����\n");
        scanf_s("%d", &action);
    }
    else
    {   // ����� �����ϴٸ� �ٵ�����
        printf("�������� �ൿ�� �����ϼ���:\n");
        printf("0: �޽�\n");
        printf("1: ����\n");
        printf("2: �ٵ��\n");
        scanf_s("%d", &action);
    }
    bm_aggro = *m_aggro;
    switch (action)
    {
    case ACTION_REST:
        // �޽�
        if (*madongseok_stamina < STM_MAX)
        {
            *madongseok_stamina += 1;
        }
        if (*m_aggro > AGGRO_MIN)
        {
            (*m_aggro) -= 1; // ��׷� ����
        }
        else
            (*m_aggro) = AGGRO_MIN;
        printf("\n�޽��� ���մϴ�.\n");
        printf("\n������: ��ġ %d (��׷�: %d -> %d, ü��: %d -> %d)\n", madongseok_position, bm_aggro, *m_aggro, b_stmaina, *madongseok_stamina);
        break;
    case ACTION_PROVOKE:
        // ����
        if (*m_aggro <= AGGRO_MAX)
        {
            *m_aggro = AGGRO_MAX; // ��׷� �ִ�ġ�� ����
        }
        printf("\n�������� �����մϴ�.\n");
        printf("\n������: ��ġ %d (��׷�: %d -> %d, ü��: %d)\n", madongseok_position, bm_aggro, *m_aggro, *madongseok_stamina);
        break;
    case ACTION_PULL:
        // �ٵ��
        if ((*m_aggro + 2) <= AGGRO_MAX) // ��׷� ����ó��
        {
            *m_aggro += 2; // ��׷� ����
        }
        else
            *m_aggro = AGGRO_MAX;
        *madongseok_stamina -= 1; // ���¹̳� ����
        printf("�������� �ٵ�⸦ �õ��մϴ�. ");
        double success_probability = (double)(rand() % 100); // 0���� 99������ ���� �����ϱ�
        if (success_probability < (100 - probability))
        {
            printf("�����߽��ϴ�! ����� ���� �� �̵� �Ұ� �����Դϴ�.\n");
            printf("������: ��ġ %d (��׷�: %d -> %d, ü��: %d -> %d)\n", madongseok_position, bm_aggro, *m_aggro, b_stmaina, *madongseok_stamina);
            *zombie_held = 1; // ���� ������ 
        }
        else
        {
            printf("�����߽��ϴ�.\n");
            printf("������: ��ġ %d (��׷�: %d -> %d, ü��: %d -> %d)\n", madongseok_position, bm_aggro, *m_aggro, b_stmaina, *madongseok_stamina);
        }
        break;
    default:
        printf("�߸��� �Է��Դϴ�. �ٽ� �������ּ���.\n");
        break;
    }
    if (*madongseok_stamina < STM_MIN) // ���¹̳� ���� ó��
    {
        *madongseok_stamina = STM_MIN;
    }
    else if (*madongseok_stamina > STM_MAX)
    {
        *madongseok_stamina = STM_MAX;
    }
}

int main()
{
    srand((unsigned int)time(NULL));

    printf("****************************\n");
    printf("* �̹� ������ �λ����Դϴ�.*\n");
    printf("****************************\n");
    printf("\n �ƴ� ���� ���� !? !!...\n");
    printf("\n �ٵ� ������ ��� !!!\n");
    printf("\n ������ �����ϰڽ��ϴ�.\n");
    printf("\n Code by SonYeongJin.\n");

    int train_length;
    double probability;
    int madongseok_stamina;
    do
    {
        printf("������ ���̸� �Է��ϼ���(%d �̻� %d ����): ", LEN_MIN, LEN_MAX);
        scanf_s("%d", &train_length);
        if (train_length < LEN_MIN || train_length > LEN_MAX)
        {
            printf("�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���.\n");
        }
    } while (train_length < LEN_MIN || train_length > LEN_MAX);

    // ������ Ȯ�� �Է� �ޱ�
    do
    {
        printf("������ Ȯ�� p(%%)�� �Է��ϼ���(%d �̻� %d ����): ", PROB_MIN, PROB_MAX);
        scanf_s("%lf", &probability);
        if (probability < PROB_MIN || probability > PROB_MAX)
        {
            printf("�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���.\n");
        }
    } while (probability < PROB_MIN || probability > PROB_MAX);
    do
    {
        printf("������ ü���� �Է��ϼ���(%d �̻� %d ����): ", STM_MIN, STM_MAX);
        scanf_s("%d", &madongseok_stamina);
        if (madongseok_stamina < STM_MIN || madongseok_stamina > STM_MAX)
        {
            printf("�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���.\n");
        }
    } while (madongseok_stamina < STM_MIN || madongseok_stamina > STM_MAX);

    initializeTrain(train_length); // ���� �ʱ� ����
    printTrain(train_length);      // ���� ���� ���
    int moved_citizen = 0;
    int moved_zombie = 0;
    int moved_madongseok = 0;
    int turn = 1;
    int madongseok_action;
    int zombie_held = 0;
    int c_aggro = 1; // �ù� ��׷�
    int m_aggro = 1; // ������ ��׷�

    while (1)
    {
        printf("\n<�̵� ������>\n");
        printf("%d �� �Դϴ�.\n", turn);
        moveCitizen(probability, train_length, &moved_citizen, &c_aggro); // �ù� �̵�
        printCitizenStatus(train_length, moved_citizen, turn, &c_aggro);  // �ù� ����                         
        moveZombie(probability, train_length, &moved_zombie, turn, &zombie_held, &c_aggro, &m_aggro); // ���� �̵�
        printZombieStatus(train_length, moved_zombie, turn);                                          // ���� ����
        printTrain(train_length); // ���� ����
        moveMadongseok(train_length, &moved_madongseok, &m_aggro); // ���� �̵�
        printTrain(train_length); // ���� ����
        printMadongseok(train_length, moved_madongseok, &madongseok_stamina, &m_aggro); // ���� ����

        printf("\n<�ൿ ������>\n");
        citizen_action(train_length); // �ù� �ൿ
        zombieAction(train_length, turn, &madongseok_stamina, &c_aggro, &m_aggro); // ���� �ൿ
        citizen_and_ma_lose(&madongseok_stamina, train_length); // �й� Ȯ��
        madongseokAction(train_length, &madongseok_stamina, &zombie_held, probability, &m_aggro); // ���� �׼�
        turn++; // �� ���� 2�ϸ��� ���� ��
        printf("����Ϸ��� Enter Ű�� ��������...\n");
        getchar();
        getchar(); // �ι� ȣ���� ���� ���� ���� ����ڰ� ���� ���������� ��ٸ��� ������ ���� ������ �Ѿ
    }
    return 0;
}
// 20180620 �տ���