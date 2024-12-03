/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjamil <mjamil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 15:43:22 by mjamil            #+#    #+#             */
/*   Updated: 2024/12/03 15:43:25 by mjamil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <pthread.h>  // For threads and mutexes
# include <stdio.h>    // For printf
# include <stdlib.h>   // For malloc, free, and exit
# include <unistd.h>   // For usleep and write
# include <sys/time.h> // For gettimeofday

// Macros
# define TRUE 1
# define FALSE 0
# define ERROR -1

// Struct to store philosopher's data
typedef struct s_philosopher {
    int             id;                // Philosopher ID
    pthread_t       thread;            // Thread for the philosopher
    long long       last_meal_time;    // Time since the last meal
    int             meals_eaten;       // Number of meals eaten
    struct s_data   *data;             // Pointer to shared data
} t_philosopher;

// Struct for shared data
typedef struct s_data {
    int             num_philosophers;  // Total number of philosophers
    int             time_to_die;       // Time (ms) before a philosopher dies without eating
    int             time_to_eat;       // Time (ms) taken to eat
    int             time_to_sleep;     // Time (ms) taken to sleep
    int             num_meals;         // Minimum meals each philosopher must eat (-1 if not specified)
    int             all_alive;         // Flag to check if all philosophers are alive
    long long       start_time;        // Simulation start time
    pthread_mutex_t *forks;            // Array of mutexes for forks
    pthread_mutex_t print_lock;        // Mutex to protect print statements
    t_philosopher   *philosophers;     // Array of philosophers
} t_data;



#endif
