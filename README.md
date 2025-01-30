# dcc_concurrencia
Prototipo de concurrencia
Problema:

Desarrollar una pasarela de pagos que permita a los clientes realizar pagos de forma segura y eficiente. La pasarela de pagos debe ser capaz de manejar diferentes métodos de pago, como tarjetas de crédito, PayPal, etc. Además, la pasarela de pagos debe ser capaz de manejar transacciones en diferentes monedas y países.

Requisitos:

- La pasarela de pagos debe ser capaz de manejar diferentes métodos de pago.
- La pasarela de pagos debe ser capaz de manejar transacciones en diferentes monedas y países.
- La pasarela de pagos debe ser segura y cumplir con los estándares de seguridad de la industria.
- La pasarela de pagos debe ser escalable y capaz de manejar un gran volumen de transacciones.

Prototipo:

Se propone un prototipo de pasarela de pagos que utilice diferentes modelos de concurrencia en C++ para evaluar su rendimiento y escalabilidad. A continuación, se presentan tres posibles enfoques:

1. Hilos (Threads): Utilizar la biblioteca std::thread de C++ para crear hilos que manejen las transacciones de pago de forma concurrente. Cada hilo se encargará de procesar una transacción de pago en particular.
2. Actores (Actor Model): Utilizar la biblioteca boost::actor para crear actores que representen las transacciones de pago. Los actores se comunicarán entre sí para realizar las operaciones necesarias.
