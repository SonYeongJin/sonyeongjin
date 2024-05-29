#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LEN_MIN 15  // 열차 최소 길이
#define LEN_MAX 50  // 열차 최대 길이
#define PROB_MIN 10 // 최소 확률
#define PROB_MAX 90 // 최대 확률
// 시민과 좀비를 나타내는 문자 상수
#define CITIZEN 'C'
#define ZOMBIE 'Z'
#define EMPTY_SPACE ' '
#define MADONGSEOK 'M'
// 마동석 체력
#define STM_MIN 0
#define STM_MAX 5
// 동석 이동 방향
#define MOVE_LEFT 1
#define MOVE_STAY 0
// 동석 행동
#define ACTION_REST 0
#define ACTION_PROVOKE 1
#define ACTION_PULL 2
// 어그로
#define AGGRO_MIN 1
#define AGGRO_MAX 5
// 좀비 공격 대상
#define ATK_NONE 0
#define ATK_CITIZEN 1
#define ATK_DONGSEOK 2

char train[LEN_MAX];

int bm_aggro = 1; // 마동석 이전 어그로 수치 저장
int bc_aggro = 1; // 사람 이전 어그로 수치 저장
int b_zb;         // 좀비 왼쪽 오른쪽 구분용
int b_stmaina;    // 마동석 이전 체력

// 열차 초기 설정 함수
void initializeTrain(int train_length)
{
    // 오른쪽부터 M, Z, 빈 칸 2개, C 배치해야함

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

// 열차 상태 출력 함수
void printTrain(int length)
{
    // 상단
    for (int i = 0; i < length; i++)
    {
        printf("#");
    }
    printf("\n");
    // 열차 상태
    for (int i = 0; i < length; i++)
    {
        printf("%c", train[i]);
    }
    printf("\n");
    // 하단
    for (int i = 0; i < length; i++)
    {
        printf("#");
    }
    printf("\n");
}

// 사람과 마동석이 패배할 때
void citizen_and_ma_lose(int* mst, int train_length)
{
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == ZOMBIE)
        {
            if (i == CITIZEN)
            {
                printf("시민이 좀비에게 물렸습니다. 다시 도전해보세요 !\n");
                exit(0);
            }
        }
    }
    if (*mst == STM_MIN)
    {
        printf("동석씨가 쓰러졌습니다... 다시 도전해보세요 !\n");
        exit(0);
    }
    return;
}

// 사람의 행동, 이기거나 넘어가거나
void citizen_action(int train_length)
{

    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == CITIZEN)
        {
            if (i == 1)
            {
                printf("축하드립니다. 시민이 부산헹 열차에서 탈출했습니다 !\n");
                exit(0);
            }
        }
    }
    printf("시민은 아무것도 하지 않습니다.\n");
}

// 시민 이동 함수 어그로 수치 추가,
void moveCitizen(double probability, int train_length, int* moved_citizen, int* c_aggro)
{
    bc_aggro = *c_aggro;
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == CITIZEN)
        {
            if (i == 1)
            {
                printf("축하드립니다. 시민이 부산헹 열차에서 탈출했습니다 !\n");
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
                    (*c_aggro)++; // 시민 어그로 증가
                    return;
                }
                else
                {
                    *moved_citizen = 0;
                    (*c_aggro)--; // 시민 어그로 감소
                    return;
                }
            }
        }
    }
    *moved_citizen = 0; // 이동하지 않았을 경우
}

// 좀비 이동 함수 짝수 턴에만 이동 가능하고, 마동석이 붙잡으면 못움직이고, 가장 가까운 대상을 찾거나 없다면 어그로 수치가 높은 쪽으로 이동하며, 어그로가 동일할 경우 시민쪽으로 이동.
void moveZombie(double probability, int train_length, int* moved_zombie, int turn, int* zombie_held, int* c_aggro, int* m_aggro)
{
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == ZOMBIE)
        {
            b_zb = i;
            if (*zombie_held == 1)
            {
                printf("좀비가 붙들려서 이동할 수 없습니다.\n");
                *zombie_held = 0;
                return;
            }

            if (turn % 2 == 0)
            { // 짝수 턴에 이동
                *moved_zombie = 1;
                return;
            }

            // 가장 가까운 대상(시민 또는 마동석)을 찾음
            int target_index = -1;
            int closest_aggro = -1; // 가장 높은 어그로 수치
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
                        // 어그로 수치가 같으면 시민 쪽으로 이동
                        target_index = j;
                    }
                }
            }

            // 대상이 인접하지 않은 경우에만 이동
            if (target_index != -1)
            {
                if (target_index > i && train[i + 1] == EMPTY_SPACE && train[i + 1] != MADONGSEOK)
                {
                    // 마동석(오른쪽)으로 이동 가능한 경우 이동
                    train[i + 1] = ZOMBIE;
                    train[i] = EMPTY_SPACE;
                    *moved_zombie = 1;
                    return;
                }
                else if (target_index < i && train[i - 1] == EMPTY_SPACE && train[i - 1] != CITIZEN)
                {
                    // 시민(왼쪽)으로 이동 가능한 경우 이동
                    train[i - 1] = ZOMBIE;
                    train[i] = EMPTY_SPACE;
                    *moved_zombie = 1;
                    return;
                }
            }
        }
    }
    *moved_zombie = 0; // 이동하지 않았을 경우
}

// 시민 상태 출력 + 어그로 추가
void printCitizenStatus(int train_length, int moved_citizen, int turn, int* c_aggro)
{
    int citizen_position = -1;

    // 시민의 위치 찾기
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == CITIZEN)
        {
            citizen_position = i;
            break;
        }
    }

    // 시민 상태 출력
    if (moved_citizen == 0)
    {
        if (*c_aggro < AGGRO_MIN) //시민 어그로 최소 수치 예외
        {
            *c_aggro = AGGRO_MIN;
            printf("\n시민: stay %d 어그로 : %d -> %d \n", citizen_position, bc_aggro, *c_aggro);
        }
        else
        {
            printf("\n시민: stay %d 어그로 : %d -> %d \n", citizen_position, bc_aggro, *c_aggro);
        }
    }
    else
    {
        if (*c_aggro > AGGRO_MAX) //시민 어그로 최대 수치 예외
        {
            *c_aggro = AGGRO_MAX;
            printf("\n시민: %d → %d 어그로 : %d -> %d \n", citizen_position + 1, citizen_position, bc_aggro, *c_aggro);
        }
        else
        {
            printf("\n시민: %d → %d 어그로 : %d -> %d \n", citizen_position + 1, citizen_position, bc_aggro, *c_aggro);
        }
    }
}

// 좀비 상태 출력 함수
void printZombieStatus(int train_length, int moved_zombie, int turn)
{
    int zombie_position = -1;

    // 좀비의 위치 찾기
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == ZOMBIE)
        {
            zombie_position = i;
            break;
        }
    }

    // 좀비 상태 출력
    if (turn % 2 != 0)
    {
        if (zombie_position > b_zb)
        {
            printf("좀비: %d -> %d\n", b_zb, zombie_position);
        }
        else if (zombie_position < b_zb)
        {
            printf("좀비: %d -> %d\n", b_zb, zombie_position);
        }
    }
    else if (turn % 2 == 0)
    {
        printf("좀비: stay %d (움직이지 않는 턴입니다.)\n", zombie_position);
    }
    printf("\n");
}

// 마동석 이동 함수 좀비와 인접해 있으면 대기만 뜨고, 없다면 이동 가능,
void moveMadongseok(int train_length, int* moved_madongseok, int* m_aggro)
{
    int madongseok_position = -1;
    int move;
    int can_move_left = 1; // 왼쪽으로 이동 가능 여부
    bm_aggro = *m_aggro;
    // 마동석의 현재 위치 찾기
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == MADONGSEOK)
        {
            madongseok_position = i;
            break;
        }
    }

    // 마동석의 좌우에 좀비가 있는지 확인하기
    if ((madongseok_position > 0 && train[madongseok_position - 1] == ZOMBIE) ||
        (madongseok_position < train_length - 1 && train[madongseok_position + 1] == ZOMBIE))
    {
        can_move_left = 0;
    }

    // 이동 입력받기
    do
    {
        printf("마동석의 이동을 선택하세요:\n");
        printf("0: 제자리 대기\n");
        if (can_move_left)
        {
            printf("1: 왼쪽으로 이동\n");
        }
        scanf_s("%d", &move);

        if (move != MOVE_STAY && move != MOVE_LEFT)
        {
            printf("잘못된 입력입니다. 다시 시도하세요.\n");
        }
        else if (move == MOVE_LEFT && !can_move_left)
        {
            printf("왼쪽으로 이동할 수 없습니다. 다시 시도하세요.\n");
        }

    } while ((move != MOVE_STAY && move != MOVE_LEFT) || (move == MOVE_LEFT && !can_move_left));

    if (move == MOVE_STAY)
    {
        // 제자리 대기하기
        if (*m_aggro > AGGRO_MIN) // 어그로 줄이고, 최소 수치 예외 설정
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
            if (*m_aggro >= AGGRO_MAX) // 어그로 추가하고, 최대 수치 예외 설정
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
            printf("왼쪽으로 이동할 수 없습니다.\n");
            *moved_madongseok = 0;
        }
    }
}
// 마동석 상태 출력하기, 스태미나, 어그로 수치 포함
void printMadongseok(int train_length, int moved_madongseok, int* madongseok_stamina, int* m_aggro)
{
    int madongseok_position = -1;
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == MADONGSEOK)
        {
            madongseok_position = i; // 동석 위치 찾기
            break;
        }
    }
    if (*madongseok_stamina == STM_MIN)
    {
        printf("동석씨가 쓰러졌습니다... 다시 도전해보세요 !\n"); // 마동석 스태미너 줄어들 시
        exit(0);
    }
    if (moved_madongseok == 1)
    {
        printf("\n마동석: 위치 %d -> %d (어그로: %d -> %d, 체력: %d)\n", madongseok_position + 1, madongseok_position, bm_aggro, *m_aggro, *madongseok_stamina);
    }
    else if (moved_madongseok == 0)
    {
        printf("\n 마동석: stay %d (어그로: %d -> %d, 체력: %d)\n", madongseok_position, bm_aggro, *m_aggro, *madongseok_stamina);
    }
}

//좀비 행동, 한 쪽에 사람 및 마동석이 인접해 있을 경우에 공격하고, 양 쪽에 인접해있다면 어그로 수치가 높은 쪽으로 공격, 아무도 없으면 넘어감
void zombieAction(int train_length, int turn, int* madongseok_stamina, int* c_aggro, int* m_aggro)
{
    int b_stamina = *madongseok_stamina; // 스태미너 변화 전 수치 저장
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == ZOMBIE)
        {
            int left_target_index = -1;  // 왼쪽 공격 대상 위치
            int right_target_index = -1; // 오른쪽 공격 대상 위치

            // 왼쪽 인접한 사람 확인
            if (i > 0 && (train[i - 1] == CITIZEN || train[i - 1] == MADONGSEOK))
            {
                left_target_index = i - 1;
            }
            // 오른쪽 인접한 사람 확인
            if (i < train_length - 1 && (train[i + 1] == CITIZEN || train[i + 1] == MADONGSEOK))
            {
                right_target_index = i + 1;
            }

            // 왼쪽에 시민 또는 마동석이 있을 경우에 공격함.. 사실상 시민만...
            if (left_target_index != -1 && right_target_index == -1)
            {
                if (train[left_target_index] == CITIZEN)
                {
                    printf("시민이 좀비에게 공격 받아서 죽었습니다.");
                    exit(0);
                }
                else if (train[left_target_index] == MADONGSEOK)
                {
                    *madongseok_stamina -= 1;
                    printf("마동석이 좀비에게 공격당했습니다. 체력: %d -> %d\n", b_stamina, *madongseok_stamina);

                    if (*madongseok_stamina == STM_MIN)
                    {
                        printf("동석씨가 쓰러졌습니다... 다시 도전해보세요 !\n"); // 패배
                        exit(0);
                    }
                }
            }
            // 오른쪽에 시민 또는 마동석이 있을 경우에 공격. .. 여기도 사실상 마동석만..
            else if (left_target_index == -1 && right_target_index != -1)
            {
                if (train[right_target_index] == CITIZEN)
                {
                    printf("시민이 좀비에게 공격 받아서 죽었습니다.");
                    exit(0);
                }
                else if (train[right_target_index] == MADONGSEOK)
                {
                    *madongseok_stamina -= 1;
                    printf("마동석이 좀비에게 공격당했습니다. 체력: %d -> %d\n", b_stamina, *madongseok_stamina);

                    if (*madongseok_stamina == STM_MIN)
                    {
                        printf("동석씨가 쓰러졌습니다... 다시 도전해보세요 !\n"); // 패배
                        exit(0);
                    }
                }
            }
            // 양쪽 다 인접한 경우 어그로가 높은 쪽을 공격
            else if (left_target_index != -1 && right_target_index != -1)
            {
                int target_index = (*c_aggro > *m_aggro) ? left_target_index : right_target_index;

                if (train[target_index] == CITIZEN)
                {
                    printf("시민이 좀비에게 공격 받아서 죽었습니다.");
                    exit(0);
                }
                else if (train[target_index] == MADONGSEOK)
                {
                    *madongseok_stamina -= 1;
                    printf("마동석이 좀비에게 공격당했습니다. 어그로: %d vs. %d, 체력: %d -> %d\n", *c_aggro, *m_aggro, b_stamina, *madongseok_stamina);

                    if (*madongseok_stamina == STM_MIN)
                    {
                        printf("동석씨가 쓰러졌습니다... 다시 도전해보세요 !\n"); // 패배
                        exit(0);
                    }
                }
            }
            // 아무도 없으면 넘어감 .
            printf("공격할 대상 없습니다.\n");
        }
    }
}

//마동석 행동. 좀비와 인접하지 않다면 휴식과 도발만, 인접해 있다면 붙잡기 가능, 붙잡으면 좀비 못움직임.
void madongseokAction(int train_length, int* madongseok_stamina, int* zombie_held, int probability, int* m_aggro)
{
    int action;
    int madongseok_position = -1;
    b_stmaina = *madongseok_stamina;
    for (int i = 0; i < train_length; i++)
    {
        if (train[i] == MADONGSEOK)
        {
            madongseok_position = i; // 위치의 주소를 할당
            break;
        }
    }

    // 좀비와 인접하지 않다면 휴식, 도발
    if (madongseok_position > 0 && madongseok_position < train_length - 1 &&
        train[madongseok_position - 1] != ZOMBIE && train[madongseok_position + 1] != ZOMBIE)
    {
        printf("마동석의 행동을 선택하세요:\n");
        printf("0: 휴식\n");
        printf("1: 도발\n");
        scanf_s("%d", &action);
    }
    else
    {   // 좀비와 인접하다면 붙들기까지
        printf("마동석의 행동을 선택하세요:\n");
        printf("0: 휴식\n");
        printf("1: 도발\n");
        printf("2: 붙들기\n");
        scanf_s("%d", &action);
    }
    bm_aggro = *m_aggro;
    switch (action)
    {
    case ACTION_REST:
        // 휴식
        if (*madongseok_stamina < STM_MAX)
        {
            *madongseok_stamina += 1;
        }
        if (*m_aggro > AGGRO_MIN)
        {
            (*m_aggro) -= 1; // 어그로 감소
        }
        else
            (*m_aggro) = AGGRO_MIN;
        printf("\n휴식을 취합니다.\n");
        printf("\n마동석: 위치 %d (어그로: %d -> %d, 체력: %d -> %d)\n", madongseok_position, bm_aggro, *m_aggro, b_stmaina, *madongseok_stamina);
        break;
    case ACTION_PROVOKE:
        // 도발
        if (*m_aggro <= AGGRO_MAX)
        {
            *m_aggro = AGGRO_MAX; // 어그로 최대치로 설정
        }
        printf("\n마동석이 도발합니다.\n");
        printf("\n마동석: 위치 %d (어그로: %d -> %d, 체력: %d)\n", madongseok_position, bm_aggro, *m_aggro, *madongseok_stamina);
        break;
    case ACTION_PULL:
        // 붙들기
        if ((*m_aggro + 2) <= AGGRO_MAX) // 어그로 예외처리
        {
            *m_aggro += 2; // 어그로 증가
        }
        else
            *m_aggro = AGGRO_MAX;
        *madongseok_stamina -= 1; // 스태미너 감소
        printf("마동석이 붙들기를 시도합니다. ");
        double success_probability = (double)(rand() % 100); // 0부터 99까지의 난수 생성하기
        if (success_probability < (100 - probability))
        {
            printf("성공했습니다! 좀비는 다음 턴 이동 불가 상태입니다.\n");
            printf("마동석: 위치 %d (어그로: %d -> %d, 체력: %d -> %d)\n", madongseok_position, bm_aggro, *m_aggro, b_stmaina, *madongseok_stamina);
            *zombie_held = 1; // 좀비를 붙잡음 
        }
        else
        {
            printf("실패했습니다.\n");
            printf("마동석: 위치 %d (어그로: %d -> %d, 체력: %d -> %d)\n", madongseok_position, bm_aggro, *m_aggro, b_stmaina, *madongseok_stamina);
        }
        break;
    default:
        printf("잘못된 입력입니다. 다시 선택해주세요.\n");
        break;
    }
    if (*madongseok_stamina < STM_MIN) // 스태미너 예외 처리
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
    printf("* 이번 열차는 부산헹입니다.*\n");
    printf("****************************\n");
    printf("\n 아니 저건 좀비 !? !!...\n");
    printf("\n 다들 기차를 벗어나 !!!\n");
    printf("\n 게임을 시작하겠습니다.\n");
    printf("\n Code by SonYeongJin.\n");

    int train_length;
    double probability;
    int madongseok_stamina;
    do
    {
        printf("열차의 길이를 입력하세요(%d 이상 %d 이하): ", LEN_MIN, LEN_MAX);
        scanf_s("%d", &train_length);
        if (train_length < LEN_MIN || train_length > LEN_MAX)
        {
            printf("잘못된 입력입니다. 다시 입력해주세요.\n");
        }
    } while (train_length < LEN_MIN || train_length > LEN_MAX);

    // 열차의 확률 입력 받기
    do
    {
        printf("열차의 확률 p(%%)를 입력하세요(%d 이상 %d 이하): ", PROB_MIN, PROB_MAX);
        scanf_s("%lf", &probability);
        if (probability < PROB_MIN || probability > PROB_MAX)
        {
            printf("잘못된 입력입니다. 다시 입력해주세요.\n");
        }
    } while (probability < PROB_MIN || probability > PROB_MAX);
    do
    {
        printf("마동석 체력을 입력하세요(%d 이상 %d 이하): ", STM_MIN, STM_MAX);
        scanf_s("%d", &madongseok_stamina);
        if (madongseok_stamina < STM_MIN || madongseok_stamina > STM_MAX)
        {
            printf("잘못된 입력입니다. 다시 입력해주세요.\n");
        }
    } while (madongseok_stamina < STM_MIN || madongseok_stamina > STM_MAX);

    initializeTrain(train_length); // 열차 초기 설정
    printTrain(train_length);      // 열차 상태 출력
    int moved_citizen = 0;
    int moved_zombie = 0;
    int moved_madongseok = 0;
    int turn = 1;
    int madongseok_action;
    int zombie_held = 0;
    int c_aggro = 1; // 시민 어그로
    int m_aggro = 1; // 마동석 어그로

    while (1)
    {
        printf("\n<이동 페이즈>\n");
        printf("%d 턴 입니다.\n", turn);
        moveCitizen(probability, train_length, &moved_citizen, &c_aggro); // 시민 이동
        printCitizenStatus(train_length, moved_citizen, turn, &c_aggro);  // 시민 상태                         
        moveZombie(probability, train_length, &moved_zombie, turn, &zombie_held, &c_aggro, &m_aggro); // 좀비 이동
        printZombieStatus(train_length, moved_zombie, turn);                                          // 좀비 상태
        printTrain(train_length); // 열차 상태
        moveMadongseok(train_length, &moved_madongseok, &m_aggro); // 동석 이동
        printTrain(train_length); // 열차 상태
        printMadongseok(train_length, moved_madongseok, &madongseok_stamina, &m_aggro); // 동석 상태

        printf("\n<행동 페이즈>\n");
        citizen_action(train_length); // 시민 행동
        zombieAction(train_length, turn, &madongseok_stamina, &c_aggro, &m_aggro); // 좀비 행동
        citizen_and_ma_lose(&madongseok_stamina, train_length); // 패배 확인
        madongseokAction(train_length, &madongseok_stamina, &zombie_held, probability, &m_aggro); // 동석 액션
        turn++; // 턴 증가 2턴마다 좀비 쉼
        printf("계속하려면 Enter 키를 누르세요...\n");
        getchar();
        getchar(); // 두번 호출해 이전 버퍼 비우고 사용자가 엔터 누를때까지 기다리고 누르면 다음 턴으로 넘어감
    }
    return 0;
}
// 20180620 손영진