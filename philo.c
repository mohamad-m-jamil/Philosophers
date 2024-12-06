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
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

void init_data(t_data *data, char **av) {
    data->total_philosophers = atoi(av[1]);
    data->time_to_die = atoi(av[2]);
    data->time_to_eat = atoi(av[3]);
    data->time_to_sleep = atoi(av[4]);
    if (av[5]) {
        data->meals_required = atoi(av[5]);
    } else {
        data->meals_required = -1;
    }
    data->meals_eaten = 0;
    data->simulation_start = current_time_in_ms();
    data->stop_simulation = 0;
    data->is_anyone_dead = 0;
    pthread_mutex_init(&data->last_meal_lock, NULL);
    pthread_mutex_init(&data->meals_lock, NULL);
    pthread_mutex_init(&data->print_lock, NULL);
    pthread_mutex_init(&data->simulation_lock, NULL);
    pthread_mutex_init(&data->death_mutex, NULL);
}

t_philosopher *init_philosophers(t_data *data, pthread_mutex_t *forks) {
    t_philosopher *philosophers = malloc(sizeof(t_philosopher) * data->total_philosophers);
    if (!philosophers) {
        perror("Failed to allocate memory for philosophers");
        return NULL;
    }

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

pthread_mutex_t *init_forks(t_data *data) {
    pthread_mutex_t *forks = malloc(sizeof(pthread_mutex_t) * data->total_philosophers);
    if (!forks) {
        perror("Failed to allocate memory for forks");
        return NULL;
    }

    for (int i = 0; i < data->total_philosophers; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    return forks;
}

void philo_think(t_philosopher *philo) {
    if (philo->sim_info->stop_simulation) {
        return;
    }
    printf("Philosopher %d is thinking.\n", philo->id);
    usleep(1000);
}

void philo_eat(t_philosopher *philo) {
    if (philo->sim_info->stop_simulation) {
        return;
    }

    pthread_mutex_lock(philo->left_fork);
    printf("Philosopher %d has taken left fork.\n", philo->id);

    pthread_mutex_lock(philo->right_fork);
    printf("Philosopher %d has taken right fork.\n", philo->id);

    printf("Philosopher %d is eating.\n", philo->id);
    philo->last_meal_time = current_time_in_ms();
    usleep(philo->sim_info->time_to_eat * 1000);  // Adjusted for actual eating time

    pthread_mutex_unlock(philo->right_fork);
    pthread_mutex_unlock(philo->left_fork);
}

void philo_sleep(t_philosopher *philo) {
    if (philo->sim_info->stop_simulation) {
        return;
    }
    printf("Philosopher %d is sleeping.\n", philo->id);
    usleep(philo->sim_info->time_to_sleep * 1000);  // Adjusted for actual sleeping time
}

int check_death(t_philosopher *philo) {
    size_t current_time = current_time_in_ms();
    size_t time_since_last_meal = current_time - philo->last_meal_time;

    // If the philosopher has not eaten within the allowed time, they die
    if (time_since_last_meal > philo->sim_info->time_to_die) {
        printf("Philosopher %d has died\n", philo->id);

        // Lock the simulation to safely modify the stop_simulation flag
        pthread_mutex_lock(&philo->sim_info->simulation_lock);
        if (!philo->sim_info->stop_simulation) {
            philo->sim_info->stop_simulation = 1;  // Set the stop condition when first philosopher dies
        }
        pthread_mutex_unlock(&philo->sim_info->simulation_lock);

        pthread_mutex_lock(&philo->sim_info->death_mutex);
        philo->sim_info->is_anyone_dead = 1;
        pthread_mutex_unlock(&philo->sim_info->death_mutex);

        return 1;  // Philosopher has died
    }
    return 0;  // Philosopher is still alive
}

void *philosopher_routine(void *arg) {
    t_philosopher *philo = (t_philosopher *)arg;

    while (1) {
        // Check if simulation should stop immediately
        pthread_mutex_lock(&philo->sim_info->simulation_lock);
        if (philo->sim_info->stop_simulation) {
            pthread_mutex_unlock(&philo->sim_info->simulation_lock);
            printf("Philosopher %d is stopping due to simulation stop flag.\n", philo->id);
            break;
        }
        pthread_mutex_unlock(&philo->sim_info->simulation_lock);

        // Check if the philosopher is dead
        if (check_death(philo)) break;  // Check death condition immediately

        philo_think(philo);
        if (check_death(philo)) break;  // Check after thinking

        philo_eat(philo);
        if (check_death(philo)) break;  // Check after eating

        philo_sleep(philo);
        if (check_death(philo)) break;  // Check after sleeping
    }
    return NULL;
}

int main(int ac, char **av) {
    t_data data;
    pthread_mutex_t *forks;
    t_philosopher *philosophers;
    pthread_t *threads;

    if (ac < 5 || ac > 6) {
        printf("Usage: ./philo num_philosophers time_to_die time_to_eat time_to_sleep [meals_required]\n");
        return 1;
    }

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
        if (pthread_create(&threads[i], NULL, philosopher_routine, (void *)&philosophers[i]) != 0) {
            perror("Failed to create thread");
            free(forks);
            free(philosophers);
            free(threads);
            return 1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < data.total_philosophers; i++) {
        pthread_join(threads[i], NULL);
    }

    free(forks);
    free(philosophers);
    free(threads);

    return 0;
}