/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjamil <mjamil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 12:53:19 by mjamil            #+#    #+#             */
/*   Updated: 2024/12/14 13:26:37 by mjamil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	print_status_debug(t_philo *philo, char *str, t_status status)
{
	if (status == GOT_FORK_1)
		printf("[%10ld]\t%03d\t%s: fork [%d]\n",
			current_time() - philo->data->start_time,
			philo->id + 1, str, philo->fork[0]);
	else if (status == GOT_FORK_2)
		printf("[%10ld]\t%03d\t%s: fork [%d]\n",
			current_time() - philo->data->start_time,
			philo->id + 1, str, philo->fork[1]);
	else
		printf("[%10ld]\t%03d\t%s\n",
			current_time() - philo->data->start_time,
			philo->id + 1, str);
}

static void	write_status_debug(t_philo *philo, t_status status)
{
	if (status == DIED)
		print_status_debug(philo, "died", status);
	else if (status == EATING)
		print_status_debug(philo, "is eating", status);
	else if (status == SLEEPING)
		print_status_debug(philo, "is sleeping", status);
	else if (status == THINKING)
		print_status_debug(philo, "is thinking", status);
	else if (status == GOT_FORK_1)
		print_status_debug(philo, "has taken a fork", status);
	else if (status == GOT_FORK_2)
		print_status_debug(philo, "has taken a fork", status);
}

static void	print_status(t_philo *philo, char *str)
{
	printf("%ld %d %s\n", current_time() - philo->data->start_time,
		philo->id + 1, str);
}

void	write_status(t_philo *philo, bool reaper_report, t_status status)
{
	pthread_mutex_lock(&philo->data->write_lock);
	if (has_simulation_stopped(philo->data) == true && reaper_report == false)
	{
		pthread_mutex_unlock(&philo->data->write_lock);
		return ;
	}
	if (DEBUG_FORMATTING == true)
	{
		write_status_debug(philo, status);
		pthread_mutex_unlock(&philo->data->write_lock);
		return ;
	}
	if (status == DIED)
		print_status(philo, "died");
	else if (status == EATING)
		print_status(philo, "is eating");
	else if (status == SLEEPING)
		print_status(philo, "is sleeping");
	else if (status == THINKING)
		print_status(philo, "is thinking");
	else if (status == GOT_FORK_1 || status == GOT_FORK_2)
		print_status(philo, "has taken a fork");
	pthread_mutex_unlock(&philo->data->write_lock);
}

void	write_outcome(t_data *data)
{
	unsigned int	i;
	unsigned int	full_count;

	full_count = 0;
	i = 0;
	while (i < data->nb_of_philo)
	{
		if (data->philos[i]->times_ate >= (unsigned int)data->must_eat_count)
			full_count++;
		i++;
	}
	pthread_mutex_lock(&data->write_lock);
	printf("%d/%d philosophers had at least %d meals.\n",
		full_count, data->nb_of_philo, data->must_eat_count);
	pthread_mutex_unlock(&data->write_lock);
	return ;
}