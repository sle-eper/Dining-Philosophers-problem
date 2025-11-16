/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmanaoui <mmanaoui@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/06 20:33:47 by mmanaoui          #+#    #+#             */
/*   Updated: 2024/08/18 19:29:22 by mmanaoui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

int	valid_nbr(t_help *help, char **av)
{
	if (help->nbr_philo <= 0 || (av[5] && help->nbr_meals <= 0))
		return (0);
	if (help->time_to_die < 0 || help->time_to_eat < 0
		|| help->time_to_sleep < 0)
		return (0);
	return (1);
}

void	kill_pids(t_help *help)
{
	int	i;

	sem_wait(help->sem_dead);
	i = 0;
	while (i < help->nbr_philo)
	{
		if (help->pids[i])
			kill(help->pids[i], 9);
		i++;
	}
}

void	cleanup_semaphore(t_help *help)
{
	free(help->philo);
	free(help->pids);
	sem_close(help->sem_forks);
	sem_close(help->sem_write);
	sem_close(help->sem_eat);
	sem_close(help->sem_monitor);
	sem_unlink("forks");
	sem_unlink("write");
	sem_unlink("eat");
	sem_unlink("monitor");
}

void	go_philo(t_help *help)
{
	int	i;

	i = 0;
	while (i < help->nbr_philo)
	{
		if (is_one_philo(help->philo) == 1)
		{
			ft_msleep(help->time_to_die);
			cleanup_semaphore(help);
			sem_unlink("dead");
			sem_close(help->sem_dead);
			(free(help), exit(0));
		}
		help->philo[i].last_time_eat = get_current_time();
		help->pids[i] = fork();
		if (help->pids[i] == 0)
		{
			pthread_create(&help->philo[i].t1, NULL, monitor_die,
				&help->philo[i]);
			routine(&help->philo[i]);
			pthread_join(help->philo[i].t1, NULL);
			exit(0);
		}
		i++;
	}
}

int	main(int ac, char **av)
{
	t_help	*help;

	help = malloc(sizeof(t_help));
	if (ac == 5 || ac == 6)
	{
		if (valid_data(ac, av) == 1)
			return (free(help), printf("INVALID DATA !!!!\n"), 0);
		if (valid_args(av, help) == 1)
			return (free(help), printf("INVALID ARGUMENT !!!!\n"), 0);
		if (valid_nbr(help, av) == 0)
			return (free(help), 0);
		if (init_semaphore(help) == 1)
			return (free(help), 0);
		help->philo_meals = help->nbr_meals;
		help->flag_meals = 1;
		help->finish_meals = 0;
		init_philo(help);
		(go_philo(help), kill_pids(help));
		cleanup_semaphore(help);
	}
	else
		printf("Usage: ./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
	sem_unlink("dead");
	sem_close(help->sem_dead);
	return (free(help), 0);
}
