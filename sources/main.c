/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjamil <mjamil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 10:42:28 by mjamil            #+#    #+#             */
/*   Updated: 2024/12/14 14:42:35 by mjamil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static bool	start_sim(t_data *data)
{
	unsigned int	i;

	data->start_time = current_time() + (data->nb_of_philo * 2 * 10);
	i = 0;
	while (i < data->nb_of_philo)
	{
		if (pthread_create(&data->philos[i]->thread, NULL,
				&philosopher, data->philos[i]) != 0)
			return (error_failure("%sCould not create thread.\n", NULL, data));
		i++;
	}
	if (data->nb_of_philo > 1)
	{
		if (pthread_create(&data->grim_reaper, NULL,
				&monitor, data) != 0)
			return (error_failure("%sCould not create thread.\n", NULL, data));
	}
	return (true);
}

static void	stop_sim(t_data	*data)
{
	unsigned int	i;

	i = 0;
	while (i < data->nb_of_philo)
	{
		pthread_join(data->philos[i]->thread, NULL);
		i++;
	}
	if (data->nb_of_philo > 1)
		pthread_join(data->grim_reaper, NULL);
	if (DEBUG_FORMATTING == true && data->must_eat_count != -1)
		write_outcome(data);
	destroy_mutexes(data);
	free_table(data);
}

int	main(int ac, char **av)
{
	t_data	*data;

	if (ac < 5 || ac > 6)
	{
		printf("Usage: <number_of_philosophers> <time_to_die> <time_to_eat>\
 <time_to_sleep> [number_of_times_each_philosopher_must_eat]\n");
		return (EXIT_FAILURE);
	}
	if (!is_correct_input(ac, av))
		return (EXIT_FAILURE);
	data = init_data(ac, av, 1);
	if (!data)
		return (EXIT_FAILURE);
	if (!start_sim(data))
		return (EXIT_FAILURE);
	stop_sim(data);
	return (EXIT_SUCCESS);
}
