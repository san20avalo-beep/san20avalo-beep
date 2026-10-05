USE [bd_Producto]
GO

-- 1. Mostrar los nombres de productos en mayusculas y minusculas --
SELECT
[NombreProducto] AS [NombreOriginal],
UPPER ([NombreProducto]) AS [NombreMayusculas],
LOWER ([NombreProducto]) AS [NombreMinusculas]
FROM [dbo].[Producto];
GO

-- 2. Identificar productos cuyos nombres tengan mas de 20 caracteres --
SELECT
[IdProducto],
[CodigoProducto],
[NombreProducto],
LEN ([NombreProducto]) AS [CantidadCaracteres]
FROM [dbo].[Producto]
WHERE LEN ([NombreProducto]) > 20;
GO

-- 3. Generar etiquetas combinando el codigo y nombre del producto --
SELECT 
[IdProducto],CONCAT ('[' , [CodigoProducto], ']  - ' , [NombreProducto]) AS [EtiquetaProducto]
FROM [dbo].[Producto];
GO

-- 4. Buscar productos cuyo nombre contenga una palabra especifica --
SELECT 
[IdProducto], [CodigoProducto],[NombreProducto]
FROM [dbo].[Producto]
WHERE [NombreProducto] LIKE '%arroz%';
GO

-- 5. Mostrar el año, mes y dia de registro de cada producto --
SELECT
[IdProducto], [NombreProducto] , [fecha_creacion] AS [FechaRegistro],
YEAR ([fecha_creacion]) AS [AñoRegistro],
MONTH ([fecha_creacion]) AS [MesRegistro],
DAY ([fecha_creacion]) AS [DiaRegistro]
FROM [dbo].[Producto];
GO

-- 6. Calcular los dias transcurridos desde el registro de cada producto --
SELECT
[IdProducto], [NombreProducto] , [fecha_creacion] AS [FechaRegistro],
DATEDIFF (DAY , [fecha_creacion], GETDATE()) AS [Dias_Transcurridos]
FROM [dbo].[Producto];
GO

-- 7. Identificar productos registrados durante los ultimos 30 dias --
SELECT * 
FROM Producto
WHERE fecha_creacion >= DATEADD(DAY, -30, GETDATE());
GO

--8. Mostrar productos vencidos o proximos a vencer --
SELECT 
[IdProducto],
[CodigoProducto],
[NombreProducto],
[Lote],
[fecha_caducidad]
FROM [dbo].[Producto]
WHERE [fecha_caducidad] <= DATEADD(DAY, 30, GETDATE());
GO

-- 9. Identificar proveedores registrados durante los proximos 90 dias --
SELECT 
[IdProveedor],
[NombreComercial],
[FechaRegistro]
FROM [dbo].[Proveedor]
WHERE [FechaRegistro] >= DATEADD(DAY, -90, GETDATE());
GO

--10. Generar un reporte de productos que combine funciones de cadena, fecha y condiciones WHERE.
SELECT 
P.[IdProducto],
UPPER(P.[CodigoProducto]) AS [CodigoMayuscula],
P.[NombreProducto],
C.[Categoria] AS [NombreCategoria],
P.[Stock],
P.[fecha_creacion],
DATEDIFF(MONTH, P.[fecha_creacion], GETDATE()) AS [MesesAntiguedad]
FROM [dbo].[Producto] P
INNER JOIN [dbo].[Categoria] C ON P.[IDcategoria] = C.[IDcategoria]
WHERE P.[Stock] > 0 
AND P.[Estado] = 'Activo'
AND YEAR(P.[fecha_creacion]) >= 2025;
GO





