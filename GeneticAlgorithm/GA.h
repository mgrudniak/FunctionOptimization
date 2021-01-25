#include<iostream>
#include<random>
#include<vector>
#include"BinaryConverter.h"

using namespace std;

// hyperparameters
constexpr double probability = 0.05;
constexpr int num_of_individuals = 20;
constexpr int num_of_generations = 50;
constexpr int num_of_variables = 1;
//

class Individual {
	vector<double> x;			// n-dimensional individual
	double y;
	double integer_fitness;
	double fractional_fitness;
public:
	Individual(vector<double>& x) :x{ x }, y{}, integer_fitness{}, fractional_fitness{} {}

	vector<double>& argument() { return x; }

	double evaluation() { return y; }
	double fitness() { return integer_fitness + fractional_fitness; }
	double int_fitness() { return integer_fitness; }
	double fract_fitness() { return fractional_fitness; }

	void set_value(double value) { y = value; }
	void set_fitness(double fitness);
};
void Individual::set_fitness(double fitness) {
	integer_fitness = floor(fitness);
	fractional_fitness = fitness - integer_fitness;
}

class GA {
	int begin_of_range;
	int end_of_range;
public:
	vector<Individual> population;
	vector<Individual> transitive_population;
	vector<double> vec_fitness;

	GA() :begin_of_range{ 10 }, end_of_range{ 10 } {}
	GA(int, int);

	void set_range(int begin, int end) { begin_of_range = begin; end_of_range = end; }

	void fitness();
	void crossover(Individual&, Individual&);
	void reproduction();
	void mutation();

	typedef double Function(vector<double>&);
	void optimize(Function);
	void show_best();
};
GA::GA(int begin_of_range, int end_of_range)
	: begin_of_range{ begin_of_range },
	end_of_range{ end_of_range } {}

int Uniform_Mersenne(int low, int high) {
	random_device seed;
	mt19937 gen(seed());
	uniform_int_distribution<int> dist(low, high);
	return dist(gen);
}

int Partition(vector<Individual>& vi, int low, int high) {
	Individual pivot = vi[high];
	int i = low - 1;

	for (int j = low; j < high; j++) {
		if (vi[j].fitness() >= pivot.fitness()) {
			i++;
			swap(vi[i], vi[j]);
		}
	}
	swap(vi[i + 1], vi[high]);
	return i + 1;
}
void Quick_Sort(vector<Individual>& vi, int low, int high) {
	if (low < high) {

		int pi = Partition(vi, low, high);

		Quick_Sort(vi, low, pi - 1);
		Quick_Sort(vi, pi + 1, high);
	}
}

void GA::fitness() {
	double evaluation_sum{};
	for (int i = 0; i < population.size(); ++i)
		evaluation_sum += population[i].evaluation();

	for (int i = 0; i < population.size(); ++i) {
		double fit{ (population[i].evaluation() / evaluation_sum) * population.size() };
		population[i].set_fitness(fit);
	}
}
void GA::crossover(Individual& i1, Individual& i2) {
	for (int i = 0; i < num_of_variables; ++i) {
		Binary father, mother;
		father.to_binary(i1.argument()[i]);
		mother.to_binary(i2.argument()[i]);
		int num_of_bits{ int(father.bits.size()) };
		int index{ Uniform_Mersenne(0,num_of_bits - 1) };
		Binary son, daughter;
		for (int j = 0; j < index; ++j) {
			son.bits[j] = father.bits[j];
			daughter.bits[j] = mother.bits[j];
		}
		for (int j = index; j < num_of_bits; ++j) {
			son.bits[j] = mother.bits[j];
			daughter.bits[j] = father.bits[j];
		}
		double child1{ son.to_double() };
		double child2{ daughter.to_double() };
		i1.argument()[i] = child1;
		i2.argument()[i] = child2;
	}
}
void GA::reproduction() {
	for (int i = 0; i < population.size(); ++i)
		for (int j = 0; j < population[i].int_fitness(); ++j)
			if (transitive_population.size() < population.size())
				transitive_population.push_back(population[i]);

	for (int i = 0; i < population.size(); ++i)
		for (int j = 0; j < population.size() - i; ++i)
			if (population[j].fract_fitness() < population[j + 1].fract_fitness())
				swap(population[j], population[j + 1]);

	int free_space{ int(population.size() - transitive_population.size()) };
	for (int i = 0; i < free_space; ++i)
		transitive_population.push_back(population[i]);

	Quick_Sort(transitive_population, 0, transitive_population.size() - 1);

	int middle{ int(transitive_population.size() / 2) };
	for (int i = 0; i < middle; ++i)
		crossover(transitive_population[i], transitive_population[i + middle]);

	for (int i = 0; i < population.size(); ++i)
		population[i] = transitive_population[i];

	transitive_population.erase(transitive_population.begin(), transitive_population.end());
}
void GA::mutation() {
	const double mutation_range_begin{ -7 };
	const double mutation_range_end{ 7 };

	int number_of_mutations{ int(round(probability*population.size()*num_of_variables)) };

	for (int i = 0; i < number_of_mutations; ++i) {
		double mutation_value{ double(Uniform_Mersenne(10000 * mutation_range_begin, 10000 * mutation_range_end)) };
		mutation_value /= 10000;

		int pop_index{ Uniform_Mersenne(0, population.size() - 1) };// losowanie indeksu w population
		int arg_index{ Uniform_Mersenne(0,num_of_variables - 1) };	// losowanie indeksu argumentu

		double current{ population[pop_index].argument()[arg_index] };
		double mutated{ current + mutation_value };
		if (mutated > begin_of_range && mutated < end_of_range)
			population[pop_index].argument()[arg_index] = mutated;
	}
}

void GA::optimize(Function f) {
	for (int i = 0; i < num_of_individuals; ++i) {
		vector<double> vec_arguments;
		for (int j = 0; j < num_of_variables; ++j) {
			double argument{ double(Uniform_Mersenne(10000 * begin_of_range,10000 * end_of_range)) };
			argument /= 10000;
			vec_arguments.push_back(argument);
		}
		Individual ind{ vec_arguments };
		population.push_back(ind);
		population[i].set_value(f(vec_arguments));
	}
	fitness();
	Quick_Sort(population, 0, population.size() - 1);

	for (int i = 0; i < num_of_generations - 1; ++i) {
		reproduction();
		mutation();
		for (int j = 0; j < num_of_individuals; ++j) {
			vector<double> vec_arguments;
			for (int k = 0; k < num_of_variables; ++k) {
				double argument = population[j].argument()[k];
				vec_arguments.push_back(argument);
			}
			population[j].set_value(f(vec_arguments));
		}
		if (i < num_of_generations - 2) {
			fitness();
			Quick_Sort(population, 0, population.size() - 1);
		}
	}
}

void GA::show_best() {
	Individual best = population[0];
	for (int i = 0; i < num_of_individuals; i++) {
		if (population[i].evaluation() > best.evaluation())
			best = population[i];
	}
	cout << '(';
	for (int i = 0; i < num_of_variables; ++i)
		cout << best.argument()[i] << ", ";
	cout << best.evaluation() << ")\n";
}