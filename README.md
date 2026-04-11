# trabalho1arquivos

O trabalho consiste dos seguintes arquivos .h e suas implementações em arquivos .c:

func.h: define as funções referentes a cada uma das funcionalidades descritas na especificação do trabalho. Quando o usuário digita uma das funcionalidades, o arquivo main.c chama uma dessas funções definidas em func.h. Cada funcionalidade está implementada em seu próprio arquivo: func_1.c, func_2.c, etc.

definicoes.h: contém diversas definições de macros, que são parâmetros fixos da especificação, como tamanho do registro de cabeçalho, tamanho do registro de dados, quantidade de campos inteiros em um registro de dados, valor de um byte lixo, etc. Também está definido um struct que representa um registro de dados. Ele ajuda a organizar a lógica e as variáveis de todo o código. Após essas definições, estão cabeçalhos de funções auxiliares, que realizam operações simples de processamento de dados e I/O. Essas funções estão implementadas em utils.c.

datamanager.h: define diversas funções relacionadas a acesso ao arquivo, atualização, escrita e verificação de registros de dados. As implementações estão em datamanager.c.

arvore.h e tabelafuncoes.h: esses dois arquivos são usados pela árvore AVL que usamos para contabilizar a quantidade de estações diferentes e de pares (codEstacao, codProxEstacao). Como a árvore AVL guarda um item genérico, ela deve receber de funções capazes de ordenar, identificar e apagar esses itens genéricos. Os ponteiros para essas funções são reunidos em uma tabela de funções cujo endereço é armazenado pela árvore ao ser criada. A implementação da árvore está em arvore.c, enquanto as tabelas de funções são usadas em datamanager.c para criar as árvores e contabilizar os dados.

Para contar elementos distintos quaisquer, uma solução possível é colocá-los numa lista e ordenar essa lista, e então percorrer ela sequencialmente para contar quantas vezes ocorre mudança. Isso funciona pois elementos iguais ficam justapostos na lista ordenada. Entretanto, em vez de inserir todos os itens para depois ordenar, seria mais eficiente se pudéssemos inseri-los ordenadamente. O problema é que fazer isso em uma lista encadeada requer em média O(n^2) operações de passagens por nós (dereferenciar ponteiros). Por isso, optamos por árvore binária, que requer O(n log n) operações de passagens por nós para inserir n nós. Como tínhamos uma implementação de árvore AVL pronta, apenas reutilizamos fazendo leves modificações para comportar itens iguais na mesma árvore.
