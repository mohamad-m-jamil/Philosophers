/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjamil <mjamil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 21:00:55 by mjamil            #+#    #+#             */
/*   Updated: 2024/12/06 21:00:55 by mjamil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
#define PHILO_H

# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_data {
    int                 total_philosophers;
    int                 time_to_die;
    int                 time_to_eat;
    int                 time_to_sleep;
    int                 meals_required;
    int                 meals_eaten;
    size_t              simulation_start;
    int                 stop_simulation;
    pthread_mutex_t     last_meal_lock;
    pthread_mutex_t     meals_lock;
    pthread_mutex_t     print_lock;
    pthread_mutex_t     simulation_lock;
    pthread_mutex_t     death_mutex;
    int                 is_anyone_dead;
} t_data;

typedef struct s_philosopher {
    int                 id;
    int                 meals_had;
    size_t              last_meal_time;
    t_data              *sim_info;
    pthread_mutex_t     *left_fork;
    pthread_mutex_t     *right_fork;
} t_philosopher;

#endif