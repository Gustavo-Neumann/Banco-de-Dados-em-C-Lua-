-- Função para calcular o primeiro e segundo dígito verificador conforme as regras corretas
local function calcular_digito(cpf, peso_inicial, max_pos)
    local soma = 0
    for i = 1, max_pos do
        soma = soma + tonumber(cpf:sub(i, i)) * (peso_inicial - i + 1)
    end
    local resto = soma % 11
    return (resto < 2) and 0 or (11 - resto)
end

-- Validação completa do CPF conforme as regras corretas
function validate_cpf(cpf)
    -- Verifica se o CPF tem exatamente 11 dígitos
    if #cpf ~= 11 then
        return false, "CPF deve conter 11 dígitos"
    end

    -- Verifica se todos os dígitos são iguais (CPF inválido)
    if cpf:match("^([0-9])%1+$") then
        return false, "CPF inválido, todos os dígitos são iguais"
    end

    -- Calcula o primeiro dígito verificador
    local primeiro_digito = calcular_digito(cpf, 10, 9)
    print("Primeiro dígito esperado: " .. cpf:sub(10, 10))
    print("Primeiro dígito calculado: " .. primeiro_digito)
    if primeiro_digito ~= tonumber(cpf:sub(10, 10)) then
        return false, "Primeiro dígito verificador inválido"
    end

    -- Calcula o segundo dígito verificador
    local segundo_digito = calcular_digito(cpf, 11, 10)
    print("Segundo dígito esperado: " .. cpf:sub(11, 11))
    print("Segundo dígito calculado: " .. segundo_digito)
    if segundo_digito ~= tonumber(cpf:sub(11, 11)) then
        return false, "Segundo dígito verificador inválido"
    end

    return true
end

-- Função para formatar o CPF no padrão "000.000.000-00"
function format_cpf(cpf)
    return string.format("%s.%s.%s-%s", cpf:sub(1, 3), cpf:sub(4, 6), cpf:sub(7, 9), cpf:sub(10, 11))
end

-- Validação de data no formato ISO8601 (YYYY-MM-DD)
function validate_date(date)
    -- Verifica se a data está no formato YYYY-MM-DD
    if not date:match("^%d%d%d%d%-%d%d%-%d%d$") then
        return false, "Data deve estar no formato YYYY-MM-DD"
    end
    return true
end

-- Função para formatar a data no formato brasileiro "DD/MM/YYYY"
function format_date(date)
    local year, month, day = date:match("^(%d%d%d%d)%-(%d%d)%-(%d%d)$")
    return string.format("%s/%s/%s", day, month, year)
end
