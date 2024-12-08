/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjamil <mjamil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 21:12:30 by mjamil            #+#    #+#             */
/*   Updated: 2024/12/06 21:12:30 by mjamil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"



size_t current_time_in_ms() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

void print_action(t_philosopher *philo, const char *action) {
    pthread_mutex_lock(&philo->sim_info->print_lock);
    if (!philo->sim_info->stop_simulation) {
        printf("%lu Philosopher %d %s\n", current_time_in_ms() - philo->sim_info->simulation_start, philo->id, action);
    }
    pthread_mutex_unlock(&philo->sim_info->print_lock);
}

void init_data(t_data *data, char **av) {
    data->total_philosophers = atoi(av[1]);
    data->time_to_die = atoi(av[2]);
    data->time_to_eat = atoi(av[3]);
    data->time_to_sleep = atoi(av[4]);
    data->meals_required = (av[5]) ? atoi(av[5]) : -1;
    data->meals_eaten = 0;
    data->simulation_start = current_time_in_ms();
    data->stop_simulation = 0;

    pthread_mutex_init(&data->last_meal_lock, NULL);
    pthread_mutex_init(&data->meals_lock, NULL);
    pthread_mutex_init(&data->print_lock, NULL);
    pthread_mutex_init(&data->death_mutex, NULL);
}

pthread_mutex_t *init_forks(t_data *data) {
    pthread_mutex_t *forks = malloc(sizeof(pthread_mutex_t) * data->total_philosophers);
    if (!forks) return NULL;

    for (int i = 0; i < data->total_philosophers; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }
    return forks;
}

t_philosopher *init_philosophers(t_data *data, pthread_mutex_t *forks) {
    t_philosopher *philosophers = malloc(sizeof(t_philosopher) * data->total_philosophers);
    if (!philosophers) return NULL;

    for (int i = 0; i < data->total_philosophers; i++) {
        philosophers[i].id = i + 1;
        philosophers[i].meals_had = 0;
        philosophers[i].last_meal_time = data->simulation_start;
        philosophers[i].sim_info = data;
        philosophers[i].left_fork = &forks[i];
        philosophers[i].right_fork = &forks[(i + 1) % data->total_philosophers];
    }
    return philosophers;
}

void *philosopher_routine(void *arg) {
    t_philosopher *philo = (t_philosopher *)arg;

    while (!philo->sim_info->stop_simulation) {
        print_action(philo, "is thinking");
        pthread_mutex_lock(philo->left_fork);
        print_action(philo, "has taken a fork");
        pthread_mutex_lock(philo->right_fork);
        print_action(philo, "has taken a fork");

        pthread_mutex_lock(&philo->sim_info->last_meal_lock);
        philo->last_meal_time = current_time_in_ms();
        pthread_mutex_unlock(&philo->sim_info->last_meal_lock);

        print_action(philo, "is eating");
        usleep(philo->sim_info->time_to_eat * 1000);

        pthread_mutex_unlock(philo->right_fork);
        pthread_mutex_unlock(philo->left_fork);

        pthread_mutex_lock(&philo->sim_info->meals_lock);
        philo->meals_had++;
        if (philo->sim_info->meals_required != -1 && philo->meals_had >= philo->sim_info->meals_required) {
            philo->sim_info->meals_eaten++;
            if (philo->sim_info->meals_eaten >= philo->sim_info->total_philosophers) {
                philo->sim_info->stop_simulation = 1;
            }
        }
        pthread_mutex_unlock(&philo->sim_info->meals_lock);

        print_action(philo, "is sleeping");
        usleep(philo->sim_info->time_to_sleep * 1000);
    }
    return NULL;
}

void *monitor_death(void *arg) {
    t_philosopher *philosophers = (t_philosopher *)arg;
    t_data *data = philosophers[0].sim_info;

    while (!data->stop_simulation) {
        for (int i = 0; i < data->total_philosophers; i++) {
            pthread_mutex_lock(&data->last_meal_lock);
            if (current_time_in_ms() - philosophers[i].last_meal_time > data->time_to_die) {
                print_action(&philosophers[i], "has died");
                data->stop_simulation = 1;
                pthread_mutex_unlock(&data->last_meal_lock);
                return NULL;
            }
            pthread_mutex_unlock(&data->last_meal_lock);
        }
        usleep(1000);
    }
    return NULL;
}

int main(int ac, char **av) {
    if (ac < 5 || ac > 6) {
        printf("Usage: ./philo num_philosophers time_to_die time_to_eat time_to_sleep [meals_required]\n");
        return 1;
    }

    t_data data;
    pthread_mutex_t *forks;
    t_philosopher *philosophers;
    pthread_t *threads;
    pthread_t monitor_thread;

    init_data(&data, av);
    forks = init_forks(&data);
    if (!forks) return 1;

    philosophers = init_philosophers(&data, forks);
    if (!philosophers) {
        free(forks);
        return 1;
    }

    threads = malloc(sizeof(pthread_t) * data.total_philosophers);
    if (!threads) {
        free(forks);
        free(philosophers);
        return 1;
    }

    for (int i = 0; i < data.total_philosophers; i++) {
        if (pthread_create(&threads[i], NULL, philosopher_routine, &philosophers[i]) != 0) {
            data.stop_simulation = 1;
            break;
        }
    }

    if (pthread_create(&monitor_thread, NULL, monitor_death, philosophers) != 0) {
        data.stop_simulation = 1;
    }

    pthread_join(monitor_thread, NULL);

    for (int i = 0; i < data.total_philosophers; i++) {
        pthread_join(threads[i], NULL);
    }

    free(forks);
    free(philosophers);
    free(threads);
    return 0;
}
