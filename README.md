# Roteiro sobre configuração e uso de um pipeline CI/CD

Este repositório descreve um roteiro prático para configuração e uso de um **Servidor de Integração e Entrega Contínua**. O objetivo é 
proporcionar à pessoa estudante um contato real com essas práticas do DevOps no domínio de Sistemas Embarcados IoT.

**Contexto**: Considere que você faz parte de um Projeto de Sistema Embarcado para Internet das Coisas (IoT). Sua equipe é composta por profissionais e praticantes de diferentes áreas como cientistas e engenheiros da computação, engenheiros eletricistas e de controle e automação, mecatrônica, analistas de sistemas entre outros que têm interesse na área de projetos IoT. Todas essas pessoas colaboram em diferentes partes do projeto e elas estão distribuídas geograficamente. O projeto é licenciado através de licenças de software e hardware livre. Você entende que a implementação das práticas de integração e entrega contínua neste projeto podem proporcionar: maior disponibilidade aos principais contribuidores do projeto (responsáveis por avaliar os Pull Requests e realizar os Merges) para realizarem outros tipos de tarefas, promover maior qualidade ao código-fonte, incentivar a documentação contínua, obtenção de feedback dos usuários mais frequentemente, entre outros benefícios. Então, como vocês estão interessados nessas práticas, construíram um roteiro para configuração e uso de um servidor de integração e entrega contínua. Apesar de existirem diversos servidores para implementação dessa prática, vocês escolheram um serviço nativo do GitHub (GH) denominado GitHub Actions (GHA), para implementar esse servidor.

## Tarefa #1: Configurar o GitHub Actions

#### Passo 1

Crie um Token pessoal no GitHub (i) e;  faça um Fork (ii) deste projeto. (i) Para criar um Token, clique no ícone do seu perfil, vá em **Settings → Developer settings → Personal access tokens → Tokens (classic) → Generate new token**. Marque as opções `repo` e `workflows` para gerar o Token. Gere o token mínimo (7 dias) apenas para este experimento. Copie e guarde esse código (token) para usar quando o GitHub lhe pedir para usar uma senha (password). (ii) Clique no botão **Fork** no canto superior direito na página do projeto no GitHub. Ou seja, você vai configurar um servidor CI/CD na sua própria cópia do repositório.

#### Passo 2

Clone o repositório para sua máquina local, usando o seguinte comando (onde `<USER>` deve ser substituído pelo seu usuário no GitHub):

```bash
git clone https://github.com/<USER>/experiment-ci-cd.git
```

Em seguida, no diretório clonado, copie o código a seguir para um arquivo com o seguinte nome e caminho: `.github/workflows/experiment-ci-cd.yml`. Isto é, crie diretórios `.github` e depois `workflows` e salve o código abaixo no arquivo `experiment-ci-cd.yml`. Para isso, crie os arquivos em um editor de texto ou IDE na sua máquina. Utilize os comandos `mv`, `cd`, `ls`, `mkdir` e `touch` no seu terminal ou use a GUI para criar os diretórios e arquivo e configure para visualizar arquivos ocultos.

```yaml
name: experiment-ci-cd #Nome do workflow
on: 
  pull_request:
    branches:
      - main #Executar quando houver um pull_request na branch main
jobs: #Define os 3 Jobs que serão executados no workflow
  build: #Job responsável por compilar o projeto
    runs-on: ubuntu-latest #Os comandos serão executados na última versão da distribuição Ubuntu
    permissions:
      contents: write #Permissão de escrita em conteúdos
    steps:
      - name: Repo checkout #Nome da etapa que usará uma actions para clonar o repositório do GH
        uses: actions/checkout@v3 #Action reusável que clona o repo do GH na estação de trabalho

      - name: Espressif IoT Development Framework (ESP-IDF) #Interação com o ESP-IDF
        uses: espressif/esp-idf-ci-action@v1.1.0 #Action fornecida pela Espressif
        with:
          esp_idf_version: v4.4 #Versão do ESP-IDF a ser utilizada
          target: esp32 #Especifica a plataforma de compilação

      - name: Store Artifacts 
        uses: actions/upload-artifact@v3 #Armazena os artefatos gerados pela compilação
        with:
          name: experiment-ci-cd #Especifica o nome do artefato criado
          path: build/experiment-ci-cd.bin

  test: #Job responsável por compilar os testes unitários e gerar os artefatos relacionados
    runs-on: ubuntu-latest
    permissions:
      checks: write
      pull-requests: write
    needs: [build] #Job test só será executado se o Job build for bem-sucedido
    steps:
      - name: Repo checkout
        uses: actions/checkout@v3

      - name: Build tests
        run: |
          sudo apt-get install libcriterion-dev
          sudo apt-get install meson
          cd test
          gcc -o test test.c -lcriterion
          ./test --xml > test.xml
     
      - name: Show results
        uses: EnricoMi/publish-unit-test-result-action@v2
        if: always() #Sempre executa essa etapa
        with:
          files: test/test.xml #Arquivo com os resultados dos testes

  delivery:
    runs-on: ubuntu-latest
    permissions:
      contents: write
    needs: [test] #Este job depende do job "test"
    steps:
      - name: Repo checkout
        uses: actions/checkout@v3 #Clona o repositório do GitHub

      - name: Download artifacts
        uses: actions/download-artifact@v3 #Baixa os artefatos gerados no job "build"
        with:
          name: experiment-ci-cd

      - name: Create release
        uses: ncipollo/release-action@v1.12.0 #Action para criar release
        with:
          artifacts: "experiment-ci-cd.bin" #Artefato que será criado na release
          tag: 0.1.5 #Tag do release
          bodyFile: "body.md" #Arquivo contendo o corpo do release
```

Esse arquivo ativa e configura o GHA para toda vez que ocorrer um evento `pull_request` tendo como alvo a branch principal (main) do repositório. Ele realiza três jobs:

- faz a compilação (build)
- roda os testes (test)
- realiza uma entrega (delivery)

#### Passo 3

Entre no diretório criado (use o comando cd no terminal) ".../experiment-ci-cd/", realize um `add`, um `commit` e um `git push`, ou seja:

```bash
git add --all
git commit -m "Configurando o GitHub Actions"
git push origin main
```

## Tarefa #2: Criando um Pull Request (PR) com bug

Vamos introduzir um bug simples no teste exemplo e enviar um PR, para mostrar que ele não será aceito pelo pipeline CI/CD.

#### Passo 1
Vamos considerar que o sensor retornou um valor que não é esperado pelo nosso teste. Para isso, coloque o valor 45.0 na função `Test`. Por exemplo, basta alterar a linha 15 no arquivo ".../test/test.c", trocando o valor para 45.0 e salve o arquivo, como apresentado abaixo.

```diff
Test(suite_name, test_name){
    cr_assert(reasonable_values(45.0) == 1);
}
```

#### Passo 2

Após modificar o código, você deve criar um novo branch, realizar um `commit` e um `push`:

```bash
git checkout -b bug
git add --all
git commit -m "Alterando a função Test"
git push origin bug
```

#### Passo 3

Em seguida, crie um Pull Request (PR) com sua modificação. Para isso, basta acessar a seguinte URL em seu navegador: `https://github.com/<USER>/experiment-ci-cd/compare/main...bug`, onde `<USER>` deve ser substituído pelo seu usuário no GitHub. Nessa janela, você pode conferir as modificações feitas. Então, clique no botão "Create pull request" e na janela que se abrirá, você poderá colocar uma pequena descrição sobre o PR, confirme a criação do PR clicando no botão "Create pull request".

Após finalizar a criação do PR, será iniciada a pipeline, ou seja, o GHA iniciará automaticamente o fluxo de tarefas configurado no arquivo `experiment-ci-cd.yml`. Porém, dessa vez os testes não vão passar, como você poderá ver na tela que aparecerá. Você pode acompanhar o status dessa execução clicando na aba Actions do seu repositório.

Em suma, o Servidor CI/CD conseguiu alertar, de maneira automática, que existe um problema no código submetido, o que impede que ele seja integrado no branch principal do repositório.

## Tarefa #3: Criando um Pull Request (PR) com a correção

Coloque um valor aceitável para o código do teste. Para isso, coloque agora o valor 26.0 na função `Test` do arquivo test.c. Por exemplo, basta alterar a linha 15, colocando o valor para 26.0, como apresentado abaixo.

```diff
Test(suite_name, test_name){
    cr_assert(reasonable_values(26.0) == 1);
}
```
Após modificar o código, salve o arquivo e crie um novo branch para consertar o bug, realize um `add`, um `commit` e um `push`:

```bash
git checkout -b fixture
git add --all
git commit -m "Consertando a função Test"
git push origin fixture
```
Insira seu nome de usuário e senha (Token) do GH se for requerido

Em seguida, crie novamente um Pull Request (PR) com sua correção. Para isso, basta acessar a seguinte URL em seu navegador: `https://github.com/<USER>/experiment-ci-cd/compare/main...fixture`, onde `<USER>` deve ser substituído pelo seu usuário no GitHub. Nessa janela, você pode conferir as modificações feitas. Então, clique no botão "Create pull request" no canto superior direito da tela e na janela que se abrirá, você poderá colocar uma pequena descrição sobre o PR, confirme a criação do PR clicando no botão "Create pull request" no canto inferior direito da janela. Você pode acompanhar o andamento do seu pipeline clicando na aba Actions e em seguida, no nome do PR criado que estará em execução.

E, após finalizar a criação deste novo PR, será iniciada novamente uma pipeline, ou seja, o próprio GHA vai fazer o build do sistema, rodar o teste e realizará a entrega do artefato criando uma Release do seu projeto. Após criada, sua `Release` estará disponível na página inicial deste seu projeto no canto direito da sua tela.

# FIM

#### Passo-a-passo de configuração e instalação do ambiente ESP-IDF em uma distribuição GNU/Linux: <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html>


#### Esquemático do protótipo: [prototype.pdf](https://github.com/parrera/experiment-ci-cd/files/12381438/prototype.pdf)


#### Para deploy no protótipo:

```bash
$get_idf
$idf.py build
$idf.py -p /dev/ttyUSB0 flash monitor
```