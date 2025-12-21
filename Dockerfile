FROM node:18-alpine

# Install build tools for compiling C
RUN apk add --no-cache build-base gcc musl-dev

WORKDIR /app

# Copy source files
COPY *.c *.h Makefile package.json server.js ./

# Build C programs
RUN make clean && make

# Install Node dependencies
RUN npm install

# Expose port
EXPOSE 3000

# Start server
CMD ["npm", "start"]
