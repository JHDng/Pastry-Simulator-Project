
## About the project

- The project was conceived in order to apply concepts acquired during the "ALGORITHMS AND PRINCIPLES OF COMPUTER SCIENCE" course
- The programming language used is C, with relative GCC compiler
- The aim is to create a simplified simulation of a pastry with orders coming in, orders to deliver out and resources(ingredients) to manage
- An important factor is efficiency, so the project challenged my data structure and optimization knowledge to minimize the space and time required
- I personally used two Hashtables to manage both ingredients and recipes
  
## Full Project Specification

An industrial pastry shop wants to improve its order management system and assigns you the task of developing software to simulate the shop’s operations. The entire simulation occurs in discrete time steps. Assume that each command received as input takes one time step to execute. The simulation starts at time 0. The following elements must be considered in the simulation:

Ingredients of the desserts: Each identified by its name, consisting of a sequence of characters.\
The set of recipes offered by the pastry shop: Each identified by a name. Every recipe uses varying amounts of the required ingredients (indicated by an integer, in grams).\
The ingredient warehouse of the pastry shop: It stores all the ingredients used. The warehouse is replenished with new batches of ingredients based on a schedule established by the supplier. Each batch is characterized by a quantity (always in grams) and an expiration date, indicated as the time step at which the batch expires.\
The pastry shop’s customers place orders for one or more desserts through an online platform or by phone. In any case, the pastry shop immediately begins preparing the ordered desserts. The advanced (and expensive) machinery used to prepare the desserts is so fast that the preparation of any number of desserts can be assumed to occur within a single time step of the simulation. The required ingredients for each preparation are drawn from the warehouse, prioritizing batches with the nearest expiration date. If there are not enough ingredients to completely fulfill an order, the order is placed on hold. An arbitrary number of orders can be placed on hold. The pastry shop proceeds to prepare subsequent orders. Upon receiving a new batch of ingredients, the pastry shop evaluates whether it is possible to prepare orders currently on hold with the new ingredients. If so, they are prepared during the same time step. Pending orders are fulfilled in chronological order of arrival.\
Periodically, the delivery driver visits the pastry shop to pick up ready orders. Upon the driver’s arrival, the orders to be loaded are selected in chronological order of arrival. The process stops as soon as an order exceeds the remaining capacity (in grams) of the truck. Assume that the weight of each prepared dessert equals the sum of the quantities (in grams) of its ingredients. Each order is always loaded in its entirety. Once selected, the orders are loaded in descending order of weight. For orders of the same weight, they are loaded in chronological order of arrival.\
Assume that all quantities are integers greater than zero, regardless of the unit of measurement. Names, whether of ingredients or recipes, are defined over the alphabet {a, ..., z, A, ..., Z, _} and are up to 255 characters long. The simulation ends after reading the last command.

The input text file begins with a line containing two integers: the delivery driver’s periodicity and the truck’s capacity. This is followed by a sequence of commands, one per line, in the following format. All non-negative integers are 32-bit encodable.

aggiungi_ricetta ⟨recipe_name⟩ ⟨ingredient_name⟩ ⟨quantity⟩ ...\
Example: aggiungi_ricetta meringhe_della_prozia zucchero 100 albumi 100\
Adds a recipe to the catalog. The number of pairs (⟨ingredient_name⟩ ⟨quantity⟩) is arbitrary. If a recipe with the same name already exists, it is ignored.\
Expected output: aggiunta or ignorato.

rimuovi_ricetta ⟨recipe_name⟩\
Example: rimuovi_ricetta cannoncini\
Removes a recipe from the catalog. Has no effect if the recipe does not exist or if there are pending orders for it that have not yet been delivered.\
Expected output: rimossa, ordini in sospeso, or non presente.

rifornimento ⟨ingredient_name⟩ ⟨quantity⟩ ⟨expiration⟩ ...\
Example: rifornimento zucchero 200 150 farina 1000 220\
The pastry shop is replenished with a set of batches, one per ingredient. The number of batches is arbitrary.\
Expected output: rifornito.

ordine ⟨recipe_name⟩ ⟨number_of_items_ordered⟩\
Example: ordine torta_paradiso 36\
Places an order for ⟨number_of_items_ordered⟩ desserts made with the recipe ⟨recipe_name⟩.\
Expected output: accettato or rifiutato if no recipe with the specified name exists.

In addition to the above outputs, the program prints the orders loaded onto the delivery driver’s truck as a sequence of triples ⟨order_arrival_time⟩ ⟨recipe_name⟩ ⟨number_of_items_ordered⟩, one per line, in loading order. Given a delivery driver periodicity of n, the output is generated before processing the commands at time steps kn, where k ∈ {1, 2, ...}. If the truck is empty, the message camioncino vuoto is printed.
