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

int	check_simulation_stop(t_data *data);
int	print_sim_info(t_philosopher *philo, char action);
int	ft_atoi(char *nptr)
{
	int	sign;
	int	i;
	int	result;

	result = 0;
	i = 0;
	sign = 1;
	while (nptr[i] == 32 || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	if (nptr[i] == '-')
	{
		sign = -1;
		i++;
	}
	else if (nptr[i] == '+')
		i++;
	while (nptr[i] != '\0' && (nptr[i] >= '0' && nptr[i] <= '9'))
	{
		result *= 10;
		result += nptr[i] - '0';
		i++;
	}
	return (result * sign);
}

int	ft_min(int a, int b)
{
	if (a < b)
		return (a);
	return (b);
}

int	ft_max(int a, int b)
{
	if (a > b)
		return (a);
	return (b);
}

long long	current_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000LL + tv.tv_usec / 1000);
}

void	sleep_for(int duration)
{
	usleep(duration * 1000);
}

void	philo_eat(t_philosopher *philo)
{
	pthread_mutex_lock(philo->left_fork);
	pthread_mutex_lock(philo->right_fork);
	if (check_simulation_stop(philo->sim_info))
	{
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
		return ;
	}
	philo->meals_had += 1;
	pthread_mutex_lock(&philo->sim_info->last_meal_lock);
	philo->last_meal_time = print_sim_info(philo, 'E');
	pthread_mutex_unlock(&philo->sim_info->last_meal_lock);
	sleep_for(philo->sim_info->time_to_eat);
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}

void	philo_sleep(t_philosopher *philo)
{
	if (check_simulation_stop(philo->sim_info))
		return ;
	print_sim_info(philo, 'S');
	sleep_for(philo->sim_info->time_to_sleep);
}

void	philo_think(t_philosopher *philo)
{
	if (check_simulation_stop(philo->sim_info))
		return ;
	print_sim_info(philo, 'T');
}

void	init_data(t_data *data, char **av)
{
	data->total_philosophers = ft_atoi(av[1]);
	data->time_to_die = ft_atoi(av[2]);
	data->time_to_eat = ft_atoi(av[3]);
	data->time_to_sleep = ft_atoi(av[4]);
	if (av[5])
		data->meals_required = ft_atoi(av[5]);
	else
		data->meals_required = -1;
	data->meals_eaten = 0;
	data->simulation_start = current_time();
	data->stop_simulation = false;
	pthread_mutex_init(&data->last_meal_lock, NULL);
	pthread_mutex_init(&data->meals_lock, NULL);
	pthread_mutex_init(&data->print_lock, NULL);
	pthread_mutex_init(&data->simulation_lock, NULL);
}

t_philosopher	*init_philo(t_data *data, pthread_mutex_t **forks)
{
	t_philosopher	*philo;
	int				i;

	philo = (t_philosopher *)malloc(
			sizeof(t_philosopher) * data->total_philosophers);
	if (!philo)
		return (NULL);
	i = 0;
	while (i < data->total_philosophers)
	{
		philo[i].id = i + 1;
		philo[i].meals_had = 0;
		philo[i].last_meal_time = data->simulation_start;
		philo[i].left_fork = &(*forks)[ft_min(i,
				((i + 1) % data->total_philosophers))];
		philo[i].right_fork = &(*forks)[ft_max(i,
				((i + 1) % data->total_philosophers))];
		philo[i].sim_info = data;
		i++;
	}
	return (philo);
}

pthread_mutex_t	*init_forks(t_data *data)
{
	pthread_mutex_t	*forks;
	int				i;					

	i = -1;
	forks = malloc(sizeof(pthread_mutex_t) * data->total_philosophers);
	if (!forks)
		return (NULL);
	while (++i < data->total_philosophers)
	{
		pthread_mutex_init(&forks[i], NULL);
	}
	return (forks);
}

int	check_meal_completion(t_data *data)
{
	int	done;

	done = 0;
	pthread_mutex_lock(&data->meals_lock);
	if (data->meals_eaten == data->total_philosophers)
	{
		done = 1;
	}
	pthread_mutex_unlock(&data->meals_lock);
	return (done);
}

int	detect_starvation(t_philosopher *philo)
{
	int	tik_tak;

	pthread_mutex_lock(&philo->sim_info->last_meal_lock);
	tik_tak = current_time() - philo->last_meal_time;
	pthread_mutex_unlock(&philo->sim_info->last_meal_lock);
	if (tik_tak > philo->sim_info->time_to_die)
	{
		return (1);
	}
	return (0);
}

void	*monitor_philosophers(void *arg)
{
	t_philosopher	*philo;
	t_data			*data;
	int				i;

	philo = (t_philosopher *)arg;
	data = philo[0].sim_info;
	while (!check_meal_completion(data))
	{
		i = 0;
		while (i < data->total_philosophers)
		{
			if (detect_starvation(&philo[i]))
			{
				pthread_mutex_lock(&data->simulation_lock);
				data->stop_simulation = true;
				pthread_mutex_unlock(&data->simulation_lock);
				print_sim_info(&philo[i], 'D');
				return (NULL);
			}
			i++;
		}
		usleep(3000);
	}
	return (NULL);
}
int print_sim_info(t_philosopher *philo, char action)
{
    size_t current;
    size_t time_spent;

    current = current_time();
    time_spent = current - philo->sim_info->simulation_start;

    pthread_mutex_lock(&philo->sim_info->print_lock);

    if (action == 'F')
        printf("%-5ld %2d has taken a fork\n", time_spent, philo->id);
    else if (action == 'E')
        printf("%-5ld %2d is eating\n", time_spent, philo->id);
    else if (action == 'S')
        printf("%-5ld %2d is sleeping\n", time_spent, philo->id);
    else if (action == 'T')
        printf("%-5ld %2d is thinking\n", time_spent, philo->id);
    else if (action == 'D')
        printf("%-5ld %2d died\n", time_spent, philo->id);

    pthread_mutex_unlock(&philo->sim_info->print_lock);
    return(0);
}

int	check_simulation_stop(t_data *data)
{
	pthread_mutex_lock(&data->simulation_lock);
	if (data->stop_simulation == true)
	{
		pthread_mutex_unlock(&data->simulation_lock);
		return (1);
	}
	pthread_mutex_unlock(&data->simulation_lock);
	return (0);
}

void	*run_philosopher(void *philosopher)
{
	t_philosopher	*philo;

	philo = philosopher;
	if (philo->sim_info->total_philosophers == 1)
	{
		print_sim_info(philo, 'F');
		usleep(philo->sim_info->time_to_die * 1000);
		return (NULL);
	}
	while (!check_simulation_stop(philo->sim_info))
	{
		philo_eat(philo);
		if (philo->meals_had == philo->sim_info->meals_required)
		{
			pthread_mutex_lock(&philo->sim_info->meals_lock);
			philo->sim_info->meals_eaten += 1;
			pthread_mutex_unlock(&philo->sim_info->meals_lock);
			return (NULL);
		}
		philo_sleep(philo);
		philo_think(philo);
	}
	return (NULL);
}

void	start_simulation(t_data *data, t_philosopher *philo)
{
	pthread_t	*philo_threads;
	pthread_t	monitor_thread;
	int			i;

	philo_threads = malloc(sizeof(pthread_t) * data->total_philosophers);
	i = -1;
	while (++i < data->total_philosophers)
		pthread_create(philo_threads + i, NULL, run_philosopher, &philo[i]);
	pthread_create(&monitor_thread, NULL, monitor_philosophers, philo);
	i = -1;
	while (++i < data->total_philosophers)
		pthread_join(philo_threads[i], NULL);
	pthread_join(monitor_thread, NULL);
	free(philo_threads);
}
void	free_data(t_data *data, pthread_mutex_t *forks)
{
	int	i;

	i = 0;
	while (i < data->total_philosophers)
	{	
		pthread_mutex_destroy(&forks[i]);
		++i;
	}
	pthread_mutex_destroy(&data->print_lock);
	pthread_mutex_destroy(&data->meals_lock);
	pthread_mutex_destroy(&data->simulation_lock);
	pthread_mutex_destroy(&data->last_meal_lock);
	free(forks);
}

int	main(int argc, char **argv)
{
	t_philosopher	*philospher;
	t_data			data;
	pthread_mutex_t	*forks;

	if (argc < 5 || argc > 6)
	{
		printf("Usage: %s number_of_philosophers time_to_die time_to_eat\
		time_to_sleep [number_of_times_each_philosopher_must_eat]\n", argv[0]);
		return (1);
	}
	init_data(&data, argv);
	forks = init_forks(&data);
	philospher = init_philo(&data, &forks);
	if (!philospher)
	{
		free(forks);
		return (EXIT_FAILURE);
	}
	start_simulation(&data, philospher);
	free_data(&data, forks);
	return (0);
}
