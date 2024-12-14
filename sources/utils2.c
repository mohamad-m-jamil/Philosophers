/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjamil <mjamil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 12:05:41 by mjamil            #+#    #+#             */
/*   Updated: 2024/12/14 12:14:07 by mjamil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*free_table(t_data *data)
{
	unsigned int	i;

	if (!data)
		return (NULL);
	if (data->fork_locks != NULL)
		free(data->fork_locks);
	if (data->philos != NULL)
	{
		i = 0;
		while (i < data->nb_of_philo)
		{
			if (data->philos[i] != NULL)
				free(data->philos[i]);
			i++;
		}
		free(data->philos);
	}
	free(data);
	return (NULL);
}

void	destroy_mutexes(t_data *data)
{
	unsigned int	i;

	i = 0;
	while (i < data->nb_of_philo)
	{
		pthread_mutex_destroy(&data->fork_locks[i]);
		pthread_mutex_destroy(&data->philos[i]->meal_time_lock);
		i++;
	}
	pthread_mutex_destroy(&data->write_lock);
	pthread_mutex_destroy(&data->sim_stop_lock);
}

int	msg(char *str, char *detail, int exit_no)
{
	if (!detail)
		printf(str, "philo :");
	else
		printf(str, "philo :", detail);
	return (exit_no);
}

int	error_failure(char *str, char *details, t_data *data)
{
	if (data != NULL)
		free_table(data);
	return (msg(str, details, 0));
}

void	*error_null(char *str, char *details, t_data *data)
{
	if (data != NULL)
		free_table(data);
	msg(str, details, EXIT_FAILURE);
	return (NULL);
}
