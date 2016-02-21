/*
 * main.cpp
 *
 *  Created on: 2016年2月21日
 *      Author: AKA
 */

extern int main_server();
extern int main_client();
extern int main_test();
extern int main_test_thread();

int main(void)
{
#if defined(TARGET_SERVER)
	main_server();
#elif defined(TARGET_CLIENT)
	main_client();
#elif defined TARGET_TEST_THREAD
	main_test_thread();
#else
	main_test();
#endif
}
