// Including the required libraries

#define NUMBER_OF_IMAGES 50

// Defining global variables:

// These variables can act as a shared memory

// between different threads

// Initializing a mutex in case you wanted to use it.

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void * fetch_image(void * args)
{
    while (terminating_condition) // maybe check whether you have 50 unique images ?
    {
        png, seq <— curl(possible_args?);

        // pthread_mutex_lock( &mutex1 );  // In case you wanted to

        // guard your Critical Section with mutexes

        // Critical Section:

        // If you want to manipulate the global variables

        // and you can think of a scenario where simultaneous

        // manipulations can lead to race conditions,

        // you have to put the manipulation code in here(guarded by mutexes).

        // pthread_mutex_lock( &mutex1 );  // In case you wanted to

        // guard your Critical Section with mutexes

    }

}

int main(int argc, char *argv[]) {

    n, t <— parse(argc, argv); // using getopt

    pthread_t *p_tids = malloc(sizeof(pthread_t) * t); // initializing thread pointers

    for (int i=0; i<t; i++) {

        pthread_create(p_tids + i, NULL, fetch_image, possible_args?);  // running all of the threads

    }

    for (int i=0; i<t; i++) {

        pthread_join(p_tids[i], NULL); // waiting for the threads to finish

    }

    catpng(pngs); // combine the strips and create all.png
}














#define IMG_URL_SVR_1_IMG_1 "http://ece252-1.uwaterloo.ca:2520/image?img=1"
#define IMG_URL_SVR_1_IMG_2 "http://ece252-1.uwaterloo.ca:2520/image?img=2"
#define IMG_URL_SVR_1_IMG_3 "http://ece252-1.uwaterloo.ca:2520/image?img=3"
#define IMG_URL_SVR_2_IMG_1 "http://ece252-2.uwaterloo.ca:2520/image?img=1"
#define IMG_URL_SVR_2_IMG_2 "http://ece252-2.uwaterloo.ca:2520/image?img=2"
#define IMG_URL_SVR_2_IMG_3 "http://ece252-2.uwaterloo.ca:2520/image?img=3"
#define IMG_URL_SVR_3_IMG_1 "http://ece252-3.uwaterloo.ca:2520/image?img=1"
#define IMG_URL_SVR_3_IMG_2 "http://ece252-3.uwaterloo.ca:2520/image?img=2"
#define IMG_URL_SVR_3_IMG_3 "http://ece252-3.uwaterloo.ca:2520/image?img=3"


int choose_server(int c1, int c2, int c3) {
    int server_num;
    // 0 0 0
    if (c1 == c2 == c3){
        return server_num = 1;
    }
    // 1 0 0 
    else if (c1 == (c2 + 1) && c1 == (c3 + 1)) {
        return server_num = 2;
    }
    // 0 1 0
    else if (c2 == (c1 + 1) && c2 == (c3 + 1)) {
        return server_num = 1;
    }
    // 0 0 1
    else if (c3 == (c1 + 1) && c3 == (c2 + 1)) {
        return server_num = 1;
    }
    // 1 2 2
    else if (c2 == (c1 + 1) && c3 == (c1 + 1)) {
        server_num = 1;
    }
    // 2 1 2
    else if (c1 == (c2 + 1) && c3 == (c2 + 1)) {
        server_num = 2;
    }
    // 2 2 1
    else if (c1 == (c3 + 1) && c2 == (c3 + 1)) {
        server_num = 3;
    }
    else {
        return server_num = 1;
    }
}