# Instruções para Atualizar o GitHub

## Status Atual

✅ Projeto organizado e pronto para push:
- ✅ Firmware organizado em `firmware/`
- ✅ Interface organizada em `interface/`
- ✅ Documentação em `docs/`
- ✅ Desenhos em `desenhos/`
- ✅ README completo criado
- ✅ Arquivos desnecessários removidos
- ✅ Commit inicial feito

## Próximos Passos

### 1. Configurar Git (se necessário)

```bash
git config --global user.name "Felipe Brito"
git config --global user.email "seu-email@example.com"
```

### 2. Fazer Push para GitHub

```bash
cd /Users/brito/Desktop/Alice-TV
git push -u origin main
```

Se der erro de autenticação, use:
```bash
git push -u origin main
# Ou se a branch for master:
git push -u origin master
```

### 3. Verificar no GitHub

Acesse: https://github.com/felipebrito/TV_Alice

## Estrutura Final

```
TV_Alice/
├── firmware/
│   ├── motor_com_ir.ino      # Principal
│   ├── motor_simples.ino     # Backup
│   └── teste_ir.ino          # Teste IR
├── interface/
│   ├── interface_tv_alice.py
│   ├── simulador_tv.py
│   └── requirements.txt
├── docs/
│   ├── hardware.md
│   └── CALCULOS_SINCRONIZACAO.md
├── desenhos/
│   ├── box-all.svg
│   └── illustrator/
└── README.md
```

## Comandos Úteis

```bash
# Ver status
git status

# Adicionar mudanças
git add .

# Commit
git commit -m "Descrição das mudanças"

# Push
git push origin main
```

