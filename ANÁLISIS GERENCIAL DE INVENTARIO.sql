--1. Situación general del inventario--

-- Cantidad total de productos--
SELECT COUNT(*) AS Total_Productos
FROM Producto;

SELECT * FROM Producto;

-- Cantidad total de unidades disponibles --
SELECT SUM(Stock) AS Unidades_Disponibles
FROM Producto;

-- Precio promedio de venta --
SELECT AVG (PrecioVenta) AS PrecioPromedio_Venta
FROM Producto;

--Producto con el precio de venta más bajo--
SELECT TOP 1 NombreProducto, Descripcion, PrecioVenta
FROM Producto
ORDER BY PrecioVenta ASC;

-- Producto con el precio de venta más alto --
SELECT TOP 1 NombreProducto, Descripcion, PrecioVenta
FROM Producto
ORDER BY PrecioVenta DESC;

-- Valor económico total del inventario tomando como referencia el precio de compra --
SELECT SUM(PrecioCompra) AS ValorDe_Inventario
FROM Producto;

-- 2.Análisis por categorías --
-- Prepare un reporte que permita conocer por cada categoría --
-- Nombre de la categoría.
-- Cantidad de productos.
-- Total de unidades disponibles.
-- Precio promedio de venta.
-- Valor total del inventario.

SELECT 
    categ.Categoria,
    COUNT(prod.IdProducto) AS Cantidad_Productos,
    SUM(prod.Stock) AS Total_Unidades_Disponibles,
    AVG(prod.PrecioVenta) AS Precio_Promedio_Venta,
    SUM(prod.Stock * prod.PrecioCompra) AS Valor_Total_Inventario
FROM Producto prod
INNER JOIN Categoria categ ON prod.IDCategoria = categ.IDCategoria
GROUP BY categ.Categoria;

SELECT * FROM Categoria
SELECT * FROM Categoria_Log

-- 3.Productos con problemas de inventario
--Identifique todos los productos cuya cantidad disponible se encuentre por debajo del stock mínimo establecido.
--El reporte deberá mostrar:
--Nombre del producto.
--Categoría.
--Proveedor.
--Stock actual.
--Stock mínimo.
--Cantidad de unidades faltantes para alcanzar el stock mínimo.

SELECT 
    prod.NombreProducto,
    categ.Categoria AS NombreCategoria,
    prov.IdProveedor,
    prod.Stock AS Stock_Actual,
    prod.StockMin AS Stock_Minimo,
    (prod.StockMin - prod.Stock) AS Unidades_Faltantes
FROM Producto prod
INNER JOIN Categoria categ ON prod.IDCategoria = categ.IDCategoria
INNER JOIN Proveedor prov ON prod.IdProveedor = prov.IdProveedor
WHERE prod.Stock < prod.StockMin;

-- 4. Reporte con clasificación del inventario
SELECT 
    prod.NombreProducto,
    categ.Categoria AS NombreCategoria,
    prod.Stock AS Stock_Actual,
    prod.StockMin AS Stock_Minimo,
    CASE 
        WHEN prod.Stock < prod.StockMin THEN 'CRÍTICO'
        WHEN prod.Stock = prod.StockMin THEN 'BAJO'
        ELSE 'NORMAL'
    END AS Clasificacion
FROM Producto prod
INNER JOIN Categoria categ ON prod.IDCategoria = categ.IDCategoria;