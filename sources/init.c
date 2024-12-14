/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjamil <mjamil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 11:31:38 by mjamil            #+#    #+#             */
/*   Updated: 2024/12/14 14:39:05 by mjamil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static pthread_mutex_t	*init_forks(t_data *data)
{
	pthread_mutex_t	*forks;
	unsigned int	i;

	forks = malloc(sizeof(pthread_mutex_t) * data->nb_of_philo);
	if (!forks)
		return (error_null("%s error: Could not allocate memory.\n", NULL, 0));
	i = 0;
	while (i < data->nb_of_philo)
	{
		if (pthread_mutex_init(&forks[i], 0) != 0)
			return (error_null("%s error: Could not create mutex.\n", NULL, 0));
		i++;
	}
	return (forks);
}

static void	assign_forks(t_philo *philo)
{
	philo->fork[0] = philo->id;
	philo->fork[1] = (philo->id + 1) % philo->data->nb_of_philo;
	if (philo->id % 2)
	{
		philo->fork[0] = (philo->id + 1) % philo->data->nb_of_philo;
		philo->fork[1] = philo->id;
	}
}

static t_philo	**init_philosophers(t_data *data)
{
	t_philo			**philos;
	unsigned int	i;

	philos = malloc(sizeof(t_philo) * data->nb_of_philo);
	if (!philos)
		return (error_null("%sCould not allocate memory.\n", NULL, 0));
	i = 0;
	while (i < data->nb_of_philo)
	{
		philos[i] = malloc(sizeof(t_philo) * 1);
		if (!philos[i])
			return (error_null("%sCould not allocate memory.\n", NULL, 0));
		if (pthread_mutex_init(&philos[i]->meal_time_lock, 0) != 0)
			return (error_null("%sCould not create mutex.\n", NULL, 0));
		philos[i]->data = data;
		philos[i]->id = i;
		philos[i]->times_ate = 0;
		assign_forks(philos[i]);
		i++;
	}
	return (philos);
}

static bool	init_global_mutexes(t_data *data)
{
	data->fork_locks = init_forks(data);
	if (!data->fork_locks)
		return (false);
	if (pthread_mutex_init(&data->sim_stop_lock, 0) != 0)
		return (error_failure("%sCould not create mutex.\n", NULL, data));
	if (pthread_mutex_init(&data->write_lock, 0) != 0)
		return (error_failure("%sCould not create mutex.\n", NULL, data));
	return (true);
}

t_data	*init_data(int ac, char **av, int i)
{
	t_data	*data;

	data = malloc(sizeof(t_data) * 1);
	if (!data)
		return (error_null("%s error: Could not allocate memory.\n", NULL, 0));
	data->nb_of_philo = my_atoi(av[i++]);
	data->time_to_die = my_atoi(av[i++]);
	data->time_to_eat = my_atoi(av[i++]);
	data->time_to_sleep = my_atoi(av[i++]);
	data->must_eat_count = -1;
	if (ac - 1 == 5)
		data->must_eat_count = my_atoi(av[i]);
	data->philos = init_philosophers(data);
	if (!data->philos)
		return (NULL);
	if (!init_global_mutexes(data))
		return (NULL);
	data->sim_stop = false;
	return (data);
}
