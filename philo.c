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
long get_current_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
void print_action(t_philosopher *philo, const char *action) {
    pthread_mutex_lock(&philo->sim_info->print_lock);
    if (!philo->sim_info->stop_simulation) {
        printf("Philosopher %d %s\n", philo->id, action);
    }
    pthread_mutex_unlock(&philo->sim_info->print_lock);
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
    printf("Philosopher %d is thinking.\n", philo->id);
    usleep(1000);
}

void philo_eat(t_philosopher *philo) {
    // Lock the forks
    pthread_mutex_lock(philo->left_fork);
    pthread_mutex_lock(philo->right_fork);

    printf("Philosopher %d is eating\n", philo->id);

    // Update the last meal time
    pthread_mutex_lock(&philo->sim_info->death_mutex);
    philo->last_meal_time = current_time_in_ms();
    pthread_mutex_unlock(&philo->sim_info->death_mutex);

    // Simulate eating
    usleep(philo->sim_info->time_to_eat * 1000);

    // Unlock the forks
    pthread_mutex_unlock(philo->right_fork);
    pthread_mutex_unlock(philo->left_fork);
}


void philo_sleep(t_philosopher *philo) {
    printf("Philosopher %d is sleeping.\n", philo->id);
    usleep(1000);
}

int check_death(t_philosopher *philo) {
    size_t current_time = current_time_in_ms();
    size_t time_since_last_meal = current_time - philo->last_meal_time;

    printf("Philosopher %d last meal: %lu ms, current time: %lu ms, time to die: %d ms\n",
           philo->id, philo->last_meal_time, current_time, philo->sim_info->time_to_die);

    if (time_since_last_meal > philo->sim_info->time_to_die) {
        printf("Philosopher %d has died\n", philo->id);

        pthread_mutex_lock(&philo->sim_info->death_mutex);
        philo->sim_info->is_anyone_dead = 1; // Set the flag
        pthread_mutex_unlock(&philo->sim_info->death_mutex);

        return 1;
    }
    return 0;
}

void *philosopher_routine(void *arg) {
    t_philosopher *philo = (t_philosopher *)arg;

    while (!philo->sim_info->stop_simulation) {
        // Thinking
        print_action(philo, "is thinking");
        if (philo->sim_info->stop_simulation) break;

        // Attempt to eat
        pthread_mutex_lock(philo->left_fork);
        pthread_mutex_lock(philo->right_fork);

        if (philo->sim_info->stop_simulation) {
            pthread_mutex_unlock(philo->right_fork);
            pthread_mutex_unlock(philo->left_fork);
            break;
        }

        print_action(philo, "is eating");
        philo->last_meal_time = get_current_time();
        usleep(philo->sim_info->time_to_eat * 1000);

        pthread_mutex_unlock(philo->right_fork);
        pthread_mutex_unlock(philo->left_fork);

        // Check if simulation has been stopped
        if (philo->sim_info->stop_simulation) break;

        // Sleeping
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
            if (get_current_time() - philosophers[i].last_meal_time > data->time_to_die) {
                print_action(&philosophers[i], "has died");
                data->stop_simulation = 1;
                pthread_mutex_unlock(&data->last_meal_lock);
                return NULL;
            }
            pthread_mutex_unlock(&data->last_meal_lock);
        }
        usleep(1000); // Check periodically
    }

    return NULL;
}


int main(int ac, char **av) {
    t_data data;
    pthread_mutex_t *forks;
    t_philosopher *philosophers;
    pthread_t *threads;
    pthread_t monitor_thread;

    if (ac < 5 || ac > 6) {
        printf("Usage: ./philo num_philosophers time_to_die time_to_eat time_to_sleep [meals_required]\n");
        return 1;
    }

    // Initialize simulation data
    init_data(&data, av);

    // Initialize forks (mutexes)
    forks = init_forks(&data);
    if (!forks) {
        fprintf(stderr, "Failed to initialize forks\n");
        return 1;
    }

    // Initialize philosophers
    philosophers = init_philosophers(&data, forks);
    if (!philosophers) {
        free(forks);
        fprintf(stderr, "Failed to initialize philosophers\n");
        return 1;
    }

    // Initialize philosopher threads
    threads = malloc(sizeof(pthread_t) * data.total_philosophers);
    if (!threads) {
        free(forks);
        free(philosophers);
        fprintf(stderr, "Failed to allocate memory for threads\n");
        return 1;
    }

    for (int i = 0; i < data.total_philosophers; i++) {
        if (pthread_create(&threads[i], NULL, philosopher_routine, &philosophers[i]) != 0) {
            fprintf(stderr, "Failed to create thread for philosopher %d\n", i + 1);
            data.stop_simulation = 1; // Signal to stop the simulation
            break;
        }
    }

    // Create the monitor thread
    if (pthread_create(&monitor_thread, NULL, monitor_death, philosophers) != 0) {
        fprintf(stderr, "Failed to create death monitor thread\n");
        data.stop_simulation = 1;
    }

    // Wait for the monitor thread to finish (detects death)
    pthread_join(monitor_thread, NULL);

    // Signal all philosopher threads to stop
    data.stop_simulation = 1;

    // Wait for all philosopher threads to finish
    for (int i = 0; i < data.total_philosophers; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup resources
    free(forks);
    free(philosophers);
    free(threads);

    return 0;
}
